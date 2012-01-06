<?php

/**
 * The standard data set class
 */
abstract class EntityDefaultPartyController {
  /**
   * Used in getInstance() to know how we're loading an
   *   instance.
   */
  const FROM_PARTY_AND_ENTITY = 0;
  const FROM_PARTY = 2;
  const FROM_ENTITY = 3;
  
  /**
   * var int $delta
   *  The delta of this attached entity, the combination of delta, pid and data set name should be unique
   */
  public $delta;
  
  /**
   * var string $data_set_name
   *   The data set name (array key in hook_data_set_info)
   */
  public $data_set_name;
  
  /**
   * var $eid int the set ID
   */
  public $eid;
  
  /**
   * var object $entity 
   *   The Entity Object this wraps around
   */
  public $entity;
  
  /**
   * var $pid int the party id
   */
  public $pid;
  
  /**
   * var $party Party
   *   The Party object we're connected too.
   */
  public $party;
  
  public $attached = FALSE;
  
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
   * Constructor.
   *   Get a attached_entity object with placeholder entity and party
   * @todo: why do we need placeholders??? doesn't make sense.
   *
   * @param $data_set_name 
   *  The set name.
   * @todo don't know how best to build this
   */
  function __construct($data_set_name) {
    // If the party isn't set, this is just an entity really, so we create 
    // A "temporary" instance of the wrapper that gets saved when we attach
    // it.
    $sets = party_get_data_set_info();
    if (isset($sets[$data_set_name])) {
      $this->data_set_name = $data_set_name;
      // @todo: not sure about this, as it duplicates the set definition over 
      // and over in memory. The data set definitions should be statically cached
      // by party_get_data_set_info() (in due course!) and that function should
      // be given a parameter to get just the one set.
      $this->definition = $sets[$data_set_name];
    }
    else {
      //throw an error
    }
  }
  
  /**
   * Set the party this controller works with.
   *
   * If the entity is already set we check if this party and this entity 
   * are already attached in the database and load the Id of the attachement
   * and any other info relating to the attachement. 
   *
   * @param $party
   *  A Party object or Id.
   */   
  public function setParty($party) {
    if (is_numeric($party)) {
      $this->pid = $party;
      $this->party = reset(entity_load('party', array($party)));
    }
    else {
      $this->party = $party;
      $this->pid = isset($party->pid) ? $party->pid : NULL;
    }
    
    // If $this->entity looks like an actual entity stored in the database.
    if (isset($this->entity->{$this->getEntityIdKey()})) {
      $query = db_select('party_attached_entity', 'ds');
      $query->fields('ds');
      $query->condition('pid', $this->pid, '=');
      $query->condition('eid', $this->eid, '=');
      $result = $query->execute()->fetchAssoc();
      
      // If there is no attached in the db we unset party and treat this like it
      // hasn't been attached yet;
      if (!$result) {
        $this->attached = FALSE;
      }
      else {
        $this->attached = TRUE;
        $this->delta = $result['delta'];
      }  
    }    
  }
  
  /**
   * Set the entity this controller works with. This does not attach the Party to the entity.
   *
   * If the party is already set we check if this party and this entity 
   * are already attached in the database and load the Id of the attachement
   * and any other info relating to the attachement. 
   *
   * @param $entity Object or Id representing the $entity
   */
  public function setAttachedEntity($entity) {
    if (is_numeric($entity)) {
      $this->eid = $entity;
      $this->entity = reset(entity_load($this->getEntityType(), array($entity)));
    }
    else {
      $this->entity = $entity;
      $this->eid = $entity->{$this->getEntityIdKey()};
    }

    // Set the id of the attaching party on the entity for our view mode to use.
    if (!empty($this->pid)) {
      $this->entity->crm_attaching_party = $this->pid;
    }

    // If the Party is a Real Party, see if they're attached.
    if (isset($this->party->pid)) {
      $query = db_select('party_attached_entity', 'ds');
      $query->fields('ds');
      $query->condition('pid', $this->pid, '=');
      $query->condition('eid', $this->eid, '=');
      $result = $query->execute()->fetchAssoc();
      
      // If there is no attached in the db we unset party and treat this like it
      // hasn't been attached yet;
      if (!$result) {
        $this->attached = FALSE;
      }
      else {
        $this->attached = TRUE;
        $this->delta = $result['delta'];
      }  
    }
    
    return TRUE;
  }
  
  /**
   * Create an attached entity ready for use
   */
  function createAttachedEntity() {
    // Create as place holder entity
    $values = array();
    $bundle_key = $this->getBundleKey();
    if (is_string($bundle_key)) {
      $values[$bundle_key] = $this->getEntityBundle();
    }
    
    $this->entity = entity_create($this->definition['entity type'], $values);  
  }
  
  /**
   * Save the attached entity
   */
  function saveAttachedEntity() {
    return entity_save($this->getEntityType(), $this->entity);
  }
  
  /**
   * loadData
   * Load the information stored in the entity this class wraps around
   *
   * @return 
   *   Entity object on success, False on failure
   */
  public function loadData() {
    if (!isset($this->entity)) {
      // @todo: make this cleaner without the array faffing.
      $data = entity_load($this->getEntityType(), array($this->eid));
      $this->entity = $data[$this->eid];
    }
    
    return $this->entity; 
  }
  
  /**
   * attach
   *   attach this data set to a party
   * 
   * @param $party
   *   The party object we're attaching to.
   * @param $options
   *   A key containing the options for attaching this party.
   *    - delta (optional) Specify the delta, if this is set it will 
   *      take precedant over the method
   *    - method (optional) Specify the method, append or prepend. Defaults to append
   *      - append - put the attached entity at the end of the list
   *      - prepend - the attached entity will have delta 0   
   *
   */
  public final function attach($party, $options = array()) {
    $this->setParty($party);
    
    if ($this->attached) {
      $this->oldDelta = $this->delta;
    }
    
    // Get the next delta
    // If the delta is explicitly set...
    if (isset($options['delta'])) {
      $this->delta = $options['delta'];
    } 
    // If the method has been set...
    else if (isset($options['method'])) {
      if ($options['method'] == 'prepend') {
        $this->delta = 0;
      }
      else if ($option['method'] == 'append') {
        $this->delta = $this->getNextDelta();
      }
    }
    // Otherwise, as long as we're not already attached, we'll get the next delta available
    else if (!$this->attached) {      
      $this->delta = $this->getNextDelta();
    }
    
    $this->preAttach();
    
    // Build our relation array
    $relation = array(
      'pid' => $party->pid,
      'eid' => $this->entity->{$this->getEntityIdKey()},
      'delta' => $this->delta,
      'data_set' => $this->data_set_name,
      'entity_type' => $this->getEntityType(),
      'entity_bundle' => $this->getEntityBundle(),
    );
    
    // If this attached entity has a record stored in the database
    if ($this->attached) {    
      $tempDelta = $this->delta;
      
      // Make sure the order of deltas is correct after update 
      if ($this->oldDelta < $this->delta) {
        $this->updateAllDeltas($this->delta + 1, 'insert');
        $tempDelta = $this->delta + 1;
      }
      if ($this->oldDelta > $this->delta) {
        $this->updateAllDeltas($this->delta, 'insert');
      }
      
      // Try update first, and if that fails insert.
      $result = db_update('party_attached_entity')
        ->fields(array(
          'delta' => $tempDelta,
        ))
        ->condition('pid', $relation['pid'], '=')
        ->condition('eid', $relation['eid'], '=')
        ->condition('data_set', $this->data_set_name, '=')
        ->execute();
      
      //Remove the old delta      
      if ($this->oldDelta < $this->delta) {
        $this->updateAllDeltas($this->oldDelta, 'remove');
      }
      if ($this->oldDelta > $this->delta) {
        $this->updateAllDeltas($this->oldDelta + 1, 'remove');
      }
    }
    else {
      $this->updateAllDeltas($this->delta, 'insert');
      drupal_write_record('party_attached_entity', $relation);      
    }
    
    $this->postAttach();
  }
  
  /**
   * Do stuff before an entity gets attached
   */
  public function preAttach() {
  }
  
  /**
   * Do stuff after an entity gets attached
   */
  public function postAttach() {
  }
  
  /**
   * Detach this entity from the party.
   */
  public final function detach() {
    // Allow extending classes to add change behaviour of detach.
    $this->preDetach();
   
    db_delete('party_attached_entity')
    ->condition('eid', $this->eid, '=')
    ->condition('pid', $this->pid, '=')
    ->condition('data_set', $this->data_set_name,'=')
    ->execute();
    
    // Update all deltas
    $this->updateAllDeltas($this->delta, 'remove');
    
    $this->postDetach();
  }
  
  /**
   * Do stuff before an entity gets detached
   */
  public function preDetach() {
  }
  
  /**
   * Do stuff after an entity gets detached
   */
  public function postDetach() {
  }
  
  /**
   *  display
   *  Return the renderable array for this dataset
   * 
   *  @param $mode the display mode to use.
   */
  public function display($mode = '') {
    return $this->entity->view($mode);
  }

  /** 
   * getLabel
   * get the label of the entity this data set wraps    
   */
  public function getLabel() {
    $label = entity_label($this->definition['entity type'], $this->entity);
    return $label;
  }

  /**
   * Get actions for the attached entity.
   *
   * @return
   *  An array of links suitable for theme_links().
   */
  public function actions() {
    $actions = array(
      'edit' => array(
        'title' => 'Edit',
        'href' => 'party/' . $this->party->pid . '/' . $this->definition['path element'] . '/edit/' . $this->entity->{$this->getEntityIdKey()},
      ),
      'remove' => array(
        'title' => 'Remove',
        'href' => 'party/' . $this->party->pid . '/' . $this->definition['path element'] . '/remove/' . $this->entity->{$this->getEntityIdKey()},
      ),
    );
    return $actions;
  }  
  
  /**
   * setMain
   *   set this set instance as the main set of this type
   * @todo appears to do no such thing
   * @todo set the delta to 0
   */
  public function setMain() {
    drupal_set_message('This code uses ' . __FUNCTION__ . ' which isn\'t implemented yet', 'warning');
    return FALSE;
  }
  
  /**
   * Determine whether this is the main instance of this set for this party.
   *
   * @return
   *   True if the delta is 0.
   */
  public function isMain() {
    return ($this->delta === 0);
  }
  
  /**
   * Get the hash. This allows this attached entity to be identified exactly with a string
   *
   * @todo call it something other than hash. $entity_key?
   * 
   * At the moment, the only time this fails is if there a 2 new attached entities of the same set.
   *
   * @todo: this is not safe.
   * $this->data_set_name is already a concatenation of entity type + bundle, and that's
   * only in the cases we control.
   * Concatenation with underscores is open to ambiguity, as ENTITY + BUNDLE
   * could match a different ENTITY.
   */
  public function hash() {
    $identifier = (isset($this->entity->{$this->getEntityIdKey()})) ? $this->entity->{$this->getEntityIdKey()} : 'new';
  
    return $this->data_set_name . '__' . $identifier;
  }
  
  /**
   * Get next delta
   */
  public function getNextDelta() {
    $result = db_select('party_attached_entity', 'pae')
      ->fields('pae', array('delta'))
      ->condition('data_set', $this->data_set_name, '=')
      ->condition('pid', $this->pid, '=')
      ->orderBy('delta', 'DESC')
      ->range(0,1)
      ->execute()
      ->fetchCol();
    
    foreach($result as $delta) {
      return $delta + 1;
    }
    
    // If there was no results return 0
    return 0;
  }
  
  /**
   * Update all deltas
   * 
   * @param $delta
   *   The delta thats been insert or removed
   * @param $op
   *   The action (either insert or remove)
   */
  public function updateAllDeltas($delta, $op) {
    if (!in_array($op, array('insert','remove'))) {
      drupal_set_message("$op is not a recognised operation in " . __FUNCTION__, 'warning');
      return FALSE;
    }
    
    $query = db_update('party_attached_entity')
      ->condition('data_set', $this->data_set_name, '=')
      ->condition('pid', $this->pid, '=');
    
    if ($op = 'insert') {
       $query->expression('delta', 'delta + :one', array(':one' => 1));
       $query->condition('delta', $delta, '>=');
    }
    else if ($op = 'remove') {
      $query->expression('delta', 'delta - :one', array(':one' => 1));
      $query->condition('delta', $delta, '>');
    }
    
    $query->execute();
  }
  
  /**
   * Get the entity type we work with.
   */
  public function getEntityType() {
    return $this->definition['entity type'];
  }
  
  /**
   * Returns the entity id key of the attached entity.
   */
  private function getEntityIdKey() {
    // @todo: needs a good refactoring as probably horribly slow.
    $entity_type = $this->getEntityType();
    $entity_info = entity_get_info($entity_type);
    $key = $entity_info['entity keys']['id'];

    return $key;
  }
  
  /**
   * Get the bundle name of the attached entity.
   */
  public function getEntityBundle() {
    $bundle_key = $this->getBundleKey();
    if (!$bundle_key) {
      // If there's no bundle key, it's a singleton entity and the bundle is
      // the same as the entity type.
      $bundle = $this->getEntityType();
    }
    else {  
      // @todo: I kept this in, but really, if these two aren't the same something has gone very
      // wrong.
      if (isset($this->entity->{$bundle_key})) {
        $bundle = $this->entity->{$bundle_key};
      }
      else {
        $bundle = $this->definition['entity bundle'];
      }
    }
    return $bundle;
  }
  
  /**
   * Get the bundle key of the attached entity.
   *
   * @todo: this has no need to be here, does it?
   * we can just as easily have a help function get_entity_bundle_key($entity_type, $entity)
   * in the main module (or even better give it to EntityAPI)
   * as it never needs to be overridden.
   */
  private function getBundleKey() {
    $entity_type = $this->getEntityType();
    $info = entity_get_info($entity_type);
    return empty($info['entity keys']['bundle']) ? FALSE : $info['entity keys']['bundle'];
  }

  /**
   * get the data set type label
   */
  public function getDataSetLabel() {
    return $this->definition['label'];
  }
  
  /**
   * getTheSetDefinition
   */
  public static function getSetDefinition() {
    return $this->definition;
  }
  
  /**
   * getInstance
   *   Get an instance of the EntityDefaultPartyController class
   *
   * @param $data_set_name
   *  The name of data set.
   * @param $info
   *  The data to load the instance from. One of:
   *   - party_id
   *   - entity_id
   *   - Array(
   *      - party a party id
   *      - entity an entity id
   * @param $how
   *  The way to load the instance. This can take one of 4 values:
   *   - EntityDefaultPartyController::FROM_PARTY
   *       $info will expect a Party object or id and create an instance of the Controller with a stub entity ready for use.
   *   - EntityDefaultPartyController::FROM_ENTITY
   *       $info will expect an entity object or id and will create an instance with the given entity set as the entity to be attached
   *   - EntityDefaulyCRMController::FROM_PARTY_AND_ENTITY
   *       $info will expect an array with keys 'party' and 'entity' and will create the controller instance. 
   *
   * @return 
   *   A controller instance.
   */
  public static function getInstance($data_set_name, $info, $how = self::FROM_PARTY_AND_ENTITY) {
    $attached_entity = party_get_crm_controller($data_set_name);    
    switch ($how) {
      case self::FROM_PARTY :
        $attached_entity->setParty($info);
        $attached_entity->createAttachedEntity();
        break;
      case self::FROM_ENTITY :
        $attached_entity->setAttachedEntity($info);
        break;
      case self::FROM_PARTY_AND_ENTITY :
        $attached_entity->setParty($info['party']);
        $attached_entity->setAttachedEntity($info['entity']);
        break;
    }
    
    return $attached_entity;
  }
  
  /**
   * getInstances
   *   Get all instances attached to a party
   *
   * @param $data_set_name the name of data set.
   * @param $info The data to load the instance from
   *   Either a
   *   - party_id
   *   - entity_id
   * 
   * @param $how The way to load the instances
   *  The way to load the instances. This can take one of 2 values:
   *   - EntityDefaultPartyController::FROM_PARTY
   *       $info will expect a Party object or id and create an instance of the Controller with a stub entity ready for use.
   *   - EntityDefaultPartyController::FROM_ENTITY
   *       $info will expect an entity object or id and will create an instance with the given entity set as the entity to be attached
   */
  public static function getInstances($data_set_name, $info, $how = self::FROM_PARTY) {
    $results = array();
    
    switch($how) {
      case self::FROM_PARTY :
        $info = (is_object($info)) ? $info->pid : $info;
        $results = db_select('party_attached_entity', 'ds')
          ->fields('ds',array('delta','pid','eid'))
          ->condition('pid', $info, '=')
          ->execute()
          ->fetchAllAssoc('delta',PDO::FETCH_ASSOC);
        break;
      case self::FROM_ENTITY :
        $results = db_select('party_attached_entity', 'ds')
          ->fields('ds',array('delta','pid','eid'))
          ->condition('eid', $info, '=')
          ->execute()
          ->fetchAllAssoc('delta',PDO::FETCH_ASSOC);
        break;          
    }
    
    $attached_entities = array();
    foreach ($results as $delta => $result) {
      $info = array(
        'party' => $result['pid'],
        'entity' => $result['eid'],
      );
      $attached_entity = self::getInstance($data_set_name, $info);
      $attached_entities[$attached_entity->hash()] = $attached_entity; 
    }
    
    return $attached_entities;
  }
}
