<?php

/**
 * The standard data set class
 */
abstract class EntityDefaultCRMController {
  
  /**
   * ENTITY_TYPE
   *   The entity type this Data Set Wraps around
   */
  const ENTITY_TYPE = "";
  
  /**
   * ENTITY_ID_KEY
   *   The key of the entity unique id
   */
  const ENTITY_ID_KEY = "";
  
  /**
   * Used in getInstance() to know how we're loading an
   *   instance.
   */
  const FROM_PARTY_AND_ENTITY = 0;
  const FROM_PEID = 1;
  const FROM_PARTY = 2;
  const FROM_ENTITY = 3;
  
  /**
   * var int $id
   *   A unique identifier for each data set instance
   */
  public $id;
  
  /**
   * var string $type
   *   The data set type (array key in hook_data_set_info)
   */
  public $type;
  
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
  
  /**
   * var bool $main;
   */
  public $main;
  
  /**
   * var string $hat
   */
  public $hat;
  
  public $attached = FALSE;
  
  /**
   * construct
   *   Build this object from a set instance id
   *
   * @param $id The set instance ID
   * @todo don't know how best to build this
   */
  function __construct($type, $entity, $party = NULL) {
    // If the party isn't set, this is just an entity really, so we create 
    // A "temporary" instance of the wrapper that gets saved when we attach
    // it.
    $sets = crm_party_get_data_set_info();
    if(isset($sets[$type])) {
      $this->type = $type;
      $this->setDef = $sets[$type];
    }
    else {
      //throw an error
    }
        
    if (is_numeric($entity)) {
      $this->eid = $entity;
      $this->loadData();
    }
    elseif ($entity !== NULL) {
      $this->entity = $entity;
      $this->eid = $entity->{$this->getEntityIdKey()};
    }
    else {
      $this->eid = null;
      $this->entity = null;
    }
    
    // If the party is set we set the party and find the row in the data set table
    // to get the rest of the information
    if (isset($party)) {
      if (is_numeric($party)) {
        $this->pid = $party;
        $this->party = reset(entity_load('crm_party', array($party)));
      }
      else {
        $this->party = $party;
        $this->pid = $party->pid;
      }
      
      if ($this->entity !== NULL) {
        $query = db_select($this->getTableName(), 'ds');
        $query->fields('ds');
        $query->condition('pid', $this->pid, '=');
        $query->condition('eid', $this->eid, '=');
        $result = $query->execute()->fetchAssoc();
        
        // If there is no data set instance in the db we unset party and treat this like it
        // hasn't been attached yet;
        if (!$result) {
          $this->attached = FALSE;
        }
        else {
          $this->attached = TRUE;
          $this->id = $result['peid'];
          $this->main = $result['main'];
          $this->hat = $result['hat'];
          $this->hat_main = $result['hat_main'];
          $this->id = $result['peid'];
        }  
      }
    }    
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
   */
  public function attach($party, $options = array()) {
    if (isset($this->party) && $this->party != $party)
      return;   
    
    $relation = array(
      'pid' => $party->pid,
      'eid' => $this->entity->{$this->getEntityIdKey()},
    );

    // WIP: chuck in defaults until I figure out what these do
    $relation += array(
      'main' => isset($options['main']) ? $options['main'] : 0,
      'hat' => 'trilby',
      'hat_main' => 0,
    );
    
    // If we're setting this as the main party first remove main from
    // every other party. 
    if (isset($options['main']) AND $options['main'] == 1 AND $this->isMain() != 1) {
      $num = db_update($this->getTableName())
        ->fields(array(
          'main' => 0,
        ))
        ->condition('pid',$relation['pid'],'=')
        ->execute();
    }

    // Try update first, and if that fails insert.
    $result = db_update($this->getTableName())
      ->fields(array(
        'main' => $relation['main'],
        'hat' => $relation['hat'],
        'hat_main' => $relation['hat_main'],
      ))
      ->condition('pid',$relation['pid'],'=')
      ->condition('eid',$relation['eid'],'=')
      ->execute();
    if ($result == 0 AND  !is_numeric($this->id)) {
      drupal_write_record($this->getTableName(), $relation);
    }    
  }
  
  /**
   * form
   *   Return the form for this data set
   */
  abstract public function form(&$form, &$form_state);
  
  /**
   * form_validate
   */
  public function form_validate($form, &$form_state) {
    return;
  }
  
  /**
   * form_submit
   */
  //abstract public function form_submit($form, &$form_state);
  
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
    return $this->entity->label;
  }

  /**
   * Get the path argument for this set.
   */
   public function getPathArg() {
     return str_replace('_', '-', $this->type);
   }
   
  
  /**
   * actions
   * Return a array of action links
   */
  public function actions() {
    $actions = array(
      'edit' => array(
        'title' => 'Edit',
        'href' => 'party/' . $this->party->pid . '/edit/' . $this->type . '/' . $this->entity->{$this->getEntityIdKey()},
      ),
      'remove' => array(
        'title' => 'Remove',
        'href' => 'party/' . $this->party->pid . '/remove/' . $this->type . '/' . $this->entity->{$this->getEntityIdKey()},
      ),
    );
    return $actions;
  }  
  
  /**
   * detach
   *   detach this data from the party
   */
  public function detach() {
    db_delete($this->getTableName())
    ->condition('peid', $this->id, '=')
    ->execute();
    
    //now treat this as a "temporary data set" by unsetting the party end
    unset($this->party);
    unset($this->pid);
  }
  
  /**
   * setMain
   *   set this set instance as the main set of this type
   */
  public function setMain() {
    return $this->main;
  }
  
  /**
   * isMain
   *   is this the main instance of this set for this party
   *
   * @return
   *   True is the Set is the main one, False otherwise.
   */
  public function isMain() {
    return $this->main;
  }
  
  /**
   * setHat
   *   Set the Hat this Data Set Instance is associated wiht
   *
   * @param $hat
   *   The hat identifier
   */
  public function setHat() {
  
  }
  
  /**
   * Get Entity Type
   *
   * REQUIRES PHP 5.3.0
   * @todo find a way around this requirement
   */
  private function getEntityType() {
    $c = get_called_class();
    return $c::ENTITY_TYPE;
  }
  
  /**
   * getEntityIdKey
   *
   * REQUIRES PHP 5.3.0
   * @todo find a way around this requirement
   */
  private function getEntityIdKey() {
    $c = get_called_class();
    return $c::ENTITY_ID_KEY;
  }
  
  /**
   * getTableName
   */
  private function getTableName() {
    return 'crm_party_set_' . $this->type;
  }
  
  /**
   * getInstanceTable
   *  static method to get the table containing all the instances
   */
  public static function getInstanceTable($type) {
    return 'crm_party_set_' . $type;
  }
  
  /**
   * get the data set type label
   */
  public static function getTypeLabel($type) {
    $sets = crm_party_get_data_set_info();
    return $sets[$type]['label'];
  }
  
  /**
   * getTheSetDefinition
   */
  public static function getSetDefinition($type) {
    $sets = crm_party_get_data_set_info();
    return $sets[$type];
  }
  
  /**
   * getInstance
   *   Get an instance of the EntityDefaultCRMController class
   *
   * @param $type the type of data set.
   * @param $info The data to load the instance from
   *   Either a
   *   - peid
   *   - party_id
   *   - entity_id
   *   - Array(
   *      - party a party id
   *      - entity an entity id
   * @param $how The way to load the instance
   */
  public static function getInstance($type, $info, $how = self::FROM_PARTY_AND_ENTITY) {
    $sets = crm_party_get_data_set_info();
    // @todo: fail gracefull if no class found, eg bad URLs.
    $class = $sets[$type]['class'];
    
    switch ($how) {
      case self::FROM_PEID :
        $result = db_select(EntityDefaultCRMController::getInstanceTable($type), 'ds')
          ->fields('ds',array('pid','eid'))
          ->condition('peid', $info, '=')
          ->execute()
          ->fetchAssoc();
          
        // @todo add error handling for if record doesnt exist
        return new $class($type,$result['eid'],$result['pid']);
        break;
      case self::FROM_PARTY :
        // @todo Make some kind of proccess for adding data sets for entities that
        // dont exist yet.
        break;
      case self::FROM_ENTITY :
        return new $class($type,$info);
        break;
      case self::FROM_PARTY_AND_ENTITY :
        return new $class($type,$info['entity'],$info['party']);
        break;
    }
  }
  
  /**
   * getInstances
   *   Get all instances attached to a party
   *
   * @param $type the type of data set.
   * @param $info The data to load the instance from
   *   Either a
   *   - party_id
   *   - entity_id
   * 
   * @param $how The way to load the instances
   */
  public static function getInstances($type, $info, $how = self::FROM_PARTY) {
    $sets = crm_party_get_data_set_info();
    $class = $sets[$type]['class'];
    $results = array();
    
    switch($how) {
      case self::FROM_PARTY :
        $info = (is_object($info)) ? $info->pid : $info;
        $results = db_select(EntityDefaultCRMController::getInstanceTable($type), 'ds')
          ->fields('ds',array('peid','pid','eid'))
          ->condition('pid', $info, '=')
          ->execute()
          ->fetchAllAssoc('peid',PDO::FETCH_ASSOC);
        break;
      case self::FROM_ENTITY :
        $results = db_select(EntityDefaultCRMController::getInstanceTable($type), 'ds')
          ->fields('ds',array('peid','pid','eid'))
          ->condition('eid', $info, '=')
          ->execute()
          ->fetchAllAssoc('peid',PDO::FETCH_ASSOC);
        break;          
    }
    
    $dataSets = array();
    foreach ($results as $peid => $result) {
      $info = array(
        'party' => $result['pid'],
        'entity' => $result['eid'],
      );
      $dataSets[$peid] = self::getInstance($type, $info);
    }
    
    return $dataSets;
  }
}