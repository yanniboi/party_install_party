<?php

/**
 * Class PartyData
 *
 * This stores the entities list and party information for each Party-PartyData
 * couple.
 */
class PartyData {

  /**
   * var string $data_set_name
   *   The data set name (array key in hook_data_set_info)
   */
  protected $data_set_name;

  /**
   * var array $entities
   *   An array of attatched entities, key'd by delta
   */
  protected $entities = array();

  /**
   * var Party $party
   *   The Party object we're connected to.
   */
  protected $party;

  /**
   * Alters entity info for entities requesting CRM integration.
   *
   * @param $entity_info
   *  The info array for a single entity.
   *  Apparently, you can pass this in from the original array in an
   *  implementation of hook_entity_info_alter, thus:
   *    EntityDefaultPartyController::hook_entity_info_alter($entity_info['profile2']);
   */
  static function hook_entity_info_alter(&$entity_info) {
    $entity_info['view modes']['party'] = array(
      'label' => t('Party display'),
      'custom settings' => FALSE,
    );
  }

  /**
   * Constructor
   *
   * @param string $data_set_name
   *   The data set name.
   * @param Party $party
   *   The party object.
   */
  public function __construct($data_set_name, Party $party) {
    // Check that this is a valid data set
    if (party_get_data_set_info($data_set_name)) {
      $this->data_set_name = $data_set_name;
    }
    else {
      // Return
      return;
    }

    // Check that this is a valid party
    if (!empty($party->pid)) {
      $this->party = $party;
    }
    else {
      // Return
      return;
    }

    // This is useless without knowing what is attached so let's load the
    // stubs.
    $query = db_select('party_attached_entity', 'ae');
    $query->fields('ae', array('delta', 'eid'));
    $query->condition('pid', $this->party->pid, '=');
    $query->condition('data_set', $this->data_set_name);
    $result = $query->execute();
    $this->entities = $result->fetchAllKeyed();
  }

  /**
   * Method to return the party object
   */
  public function getParty() {
    return $this->party;
  }

  /**
   * Method to get part the entity info
   *
   * @param $key
   *   This is the information you want:
   *     - 'type' the data set entity type
   *     - 'bundle' the data set entity bundle
   *     - 'id key' the entity id key
   *     - 'bundle key' the entity bundle key
   */
  public function getDataInfo($key) {
    // Get our info
    $party_data_info = party_get_data_set_info($this->data_set_name);
    $entity_info = entity_get_info($party_data_info['entity type']);

    // Return the relevant information
    switch ($key) {
      // Data Set Info
      case 'name':
        return $this->data_set_name;
      break;
      case 'entity type':
        return $party_data_info['entity type'];
      break;
      case 'entity bundle':
        return $party_data_info['entity bundle'];
      break;

      // Entity Info
      case 'id key':
        return $entity_info['entity keys']['id'];
      break;
      case 'bundle key':
        return $entity_info['entity keys']['bundle'];
      break;
    }
  }

  /**
   * Method to load full entities
   *
   * @param mixed $deltas
   *   (optional) An array of delta(s) to load or NULL to load all entities.
   *
   * @return this
   *   For chaining.
   */
  public function loadEntities($deltas = NULL) {
    // Get our array of deltas
    if ($deltas === NULL) {
      // Load all entities
      $deltas = array_keys($this->entities);
    }

    // Iterate over our deltas loading our entities if required
    foreach ($deltas as $delta) {
      if (isset($this->entities[$delta]) && !is_object($this->entities[$delta])) {
        // This entity needs loading
        $this->entities[$delta] = reset(entity_load($this->getDataInfo('entity type'), array($this->entities[$delta])));
        $this->entities[$delta]->data_set_name = $this->data_set_name;
      }
    }

    return $this;
  }

  /**
   * Method to get an attached entity
   *
   * @param $delta
   *   (optional) A delta to get. Defaults to 0.
   * @param boolean $create
   *   (optional) Create an entity if it doesn't exist. Defaults to FALSE.
   *
   * @return mixed
   *   An entity object or one doesn't exist and we're not creating, FALSE.
   */
  public function getEntity($delta = 0, $create = FALSE) {
    // If this delta exists, check it's loaded and return it
    if (isset($this->entities[$delta])) {
      if (!is_object($this->entities[$delta])) {
        $this->loadEntities(array($delta));
      }
    }
    else {
      // If we're creating an entity, let's do that, including saving and
      // attaching it. If developers want to be able to create entities
      // without automatically attaching, they can make use of createEntity()
      // and attachEntity() manually.
      if ($create) {
        $entity = $this->createEntity();
        $this->attachEntity($entity, $delta);
        $this->save(TRUE);
      }
      else {
        // Otherwise we return false
        return FALSE;
      }
    }

    return $this->entities[$delta];
  }

  /**
   * Method to return all attached entities
   */
  public function getEntities() {
    $this->loadEntities();

    return $this->entities;
  }

  /**
   * Method to get entity ids
   *
   * This allows you to get the ids of all the entities without having to load
   * them. If they are already loaded, we extract the id.
   *
   * @return array
   *   Array of entity ids key'd by delta
   */
  public function getEntityIds() {
    $ids = array();

    foreach ($this->entities as $delta => $entity) {
      if (is_object($entity)) {
        $ids[$delta] = $entity->{$this->getDataInfo('id key')};
      }
      else {
        // We've not loaded, so pull the id
        $ids[$delta] = $entity;
      }
    }

    return $ids;
  }

  /**
   * Method to create a new entity
   */
  public function createEntity() {
   // Create a placeholder entity
    $values = array();
    if ($this->getDataInfo('bundle key')) {
      $values[$this->getDataInfo('bundle key')] = $this->getDataInfo('entity bundle');
    }

    $entity = entity_create($this->getDataInfo('entity type'), $values);
    $entity->data_set_name = $this->data_set_name;
    return $entity;
  }

  /**
   * Method to attach an entity
   *
   * Attach an Entity to the Party. This method puts the entity in the right
   * place in the $entities array and this is then saved with the "commit" method.
   *
   * @param $entity
   *   The $entity we're attaching
   * @param $method
   *   The $method we're using can be one of
   *   - append (default) - the entity will be added to the end of the list
   *   - prepend
   *   - insert
   * @param $delta
   *   If the $method is set to insert. This is the delta of the attahced
   *   entity
   *
   * @return this
   *   For chaining.
   */
  final public function attachEntity($entity, $method = 'append', $delta = 0) {
    // Fire any pre attach logic
    $this->preAttach($entity, $method, $delta);

    switch ($method) {
      case 'append':
        $this->entities[] = $entity;
        break;
      case 'prepend':
        array_unshift($this->entities, $entity);
        break;
      case 'insert':
        // Put the entity and its availability in the right place
        array_splice($this->entities, $delta, 0, $entity);
        break;
    }

    // Fire any post attach logic
    $this->postAttach($entity, $method, $delta);

    return $this;
  }

  /**
   * Method for overloading of any pre attach logic
   *
   * @param $entity
   * @param $method
   * @param $delta
   *
   * @see PartyData::attachEntity()
   */
  protected function preAttach($entity, &$method, &$delta) {}

  /**
   * Method for overloading of any post attach logic
   *
   * @param $entity
   * @param $method
   * @param $delta
   *
   * @see PartyData::attachEntity()
   */
  protected function postAttach($entity, $method, $delta) {}

  /**
   * Method for detaching entities
   *
   * @param $delta
   *   The delta of the entity to detach
   * @param boolean $return
   *   Whether you want to return the detached entity or $this for chaining.
   *
   * @return mixed
   *   Depending on $return, either the detached entity or this for chaining.
   */
  final public function detachEntity($delta, $return = FALSE) {
    // Fire any pre attach logic
    $this->preDetach($delta);

    if (isset($this->entities[$delta])) {
      // Get our entity for returning if requested
      $entity = $this->entities[$delta];

      // Detach our entity
      unset($this->entities[$delta]);

      // Reset our numeric indexes
      $this->entities = array_values($this->entities);
    }
    else {
      // Can't return the entity if it didn't exist
      $entity = FALSE;
    }

    // Fire any post attach logic
    $this->postDetach($delta);

    if ($return) {
      return $entity;
    }
    else {
      return $this;
    }
  }


  /**
   * Method for overloading of any pre detach logic
   *
   * @param $delta
   *
   * @see PartyData::detachEntity()
   */
  protected function preDetach($entity, &$method, &$delta) {}

  /**
   * Method for overloading of any post detach logic
   *
   * @param $delta
   *
   * @see PartyData::detachEntity()
   */
  protected function postDetach($entity, $method, $delta) {}


  /**
   * Method to save the attached entities info
   *
   * @param boolean $save_entities
   *   Whether or not to save entities as we go through them.
   *
   * @return this
   *   For chaining.
   */
  final public function save($save_entities = FALSE) {
    // Fire any pre attach logic
    $this->preSave();

    // Clear out our old bits
    $query = db_delete('party_attached_entity');
    $query->condition('pid', $this->party->pid, '=');
    $query->condition('data_set', $this->data_set_name);
    $query->execute();

    // Insert our entities
    $query = db_insert('party_attached_entity');
    $query->fields(array('pid', 'eid', 'delta', 'data_set','entity_type','entity_bundle'));
    foreach ($this->entities as $delta => &$entity) {
      if ($save_entities) {
        // Save our entities as we go
        entity_save($this->getDataInfo('entity type'), $entity);
      }

      $query->values(array(
        'pid' => $this->party->pid,
        'eid' => $entity->{$this->getDataInfo('id key')},
        'delta' => $delta,
        'data_set' => $this->data_set_name,
        'entity_type' => 'blah',
        'entity_bundle' => 'BLAH!',
      ));
    }
    $query->execute();

    // Fire any post attach logic
    $this->postSave();

    return $this;
  }

  /**
   * Method for overloading of any pre save logic
   *
   * @see PartyData::save()
   */
  protected function preSave() {}

  /**
   * Method for overloading of any post save logic
   *
   * @see PartyData::save()
   */
  protected function postSave() {}

  /**
   * Method to re-order attached entities
   *
   * This will re-order any entities to match the order specified in $order.
   * Any entities that are left out of $order will be appended to the new
   * order. To remove an entity, use PartyData::detachEntity().
   *
   * @param array $order
   *   A numeric array of old deltas in the new order
   *
   * @return this
   *   For chaining.
   */
  final public function reorderEntities($order) {
    // First we need to collect missed entities
    $missed_entities = array_diff_key($this->entities, array_fill_keys($order, TRUE));
    $entities = array();

    // Iterate over re-ordering our entities
    foreach ($order as $delta) {
      if (isset($this->entities[$delta])) {
        $entities[] = $this->entities[$delta];
      }
    }

    // Append any missed entities
    foreach ($missed_entities as $entity) {
      $entities[] = $entity;
    }

    // Store array values so we don't have any missed
    $this->entities = $entities;

    return $this;
  }

  /**
   * Method to shift a particular entity in the order
   *
   * @param $delta
   *   The delta of the item we're shifting
   * @param string $method
   *   The method of shift we want:
   *    - 'up' Shifts the entity up by $target
   *    - 'down' Shifts the entity down by $target
   *    - 'insert' Moves the entity to that position, shifting everything else
   *    - 'swap' Swaps the entity with the entity in $target
   * @param $target
   *   If method is 'up' or 'down', shift the elemnt up or down by this target.
   *   If method is 'swap', swap the entitiy with the entity at this delta.
   *
   * @return this
   *   For chaining.
   */
  final public function shiftEntity($delta, $method, $target = 1) {
    if (isset($this->entities[$delta])) {
      $entity = $this->entities[$delta];
      switch ($method) {
        case 'up':
          // Calculate our new position
          $newDelta = min(0, $delta - $target);
          unset($this->entities[$delta]);
          array_splice($this->entities, $newDelta, 0, array($entity));
        break;
        case 'down':
          // Calculate our new position, including removal of the moved entity
          $newDelta = $delta + $target - 1;
          unset($this->entities[$delta]);
          array_splice($this->entities, $newDelta, 0, array($entity));
        break;
        case 'insert':
          array_splice($this->entities, $target, 0, array($entity));
        break;
        case 'swap':
          // Make sure we have the other entity if it exists
          if (isset($this->entities[$target])) {
            $otherEntity = $this->entities[$target];
          }

          // Set our new position for this entity
          $this->entities[$target] = $entity;

          // If we had another entity, put it back in place
          if (isset($otherEntity)) {
            $this->entities[$delta] = $otherEntity;
          }

          // Make sure our deltas are numeric and sequencial
          $this->entities = array_values($this->entities);
        break;
      }
    }

    return $this;
  }

}