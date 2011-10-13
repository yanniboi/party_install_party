<?php

/**
 * The standard data set class
 */
abstract class CRMPartyDataSet {
  
  /**
   * abstract $entity_type
   *   The entity type this Data Set Wraps around
   */
  const ENTITY_TYPE = "";
  
  /**
   * abstract $entity_id_col
   *   The entity type this Data Set Wraps around
   */
  const ENTITY_ID_KEY = "";
  
  
  /**
   * var int $id
   *   A unique identifier for each data set instance
   */
  protected $id;
  
  /**
   * var string $type
   *   The data set type (array key in hook_data_set_info)
   */
  protected $type;
  
  /**
   * var $eid int the set ID
   */
  protected $eid;
  
  /**
   * var object $entity 
   *   The Entity Object this wraps around
   */
  protected $entity;
  
  /**
   * var $pid int the party id
   */
  protected $pid;
  
  /**
   * var $party Party
   *   The Party object we're connected too.
   */
  protected $party;
  
  /**
   * var bool $main;
   */
  protected $main;
  
  /**
   * var string $hat
   */
  protected $hat;
  
  /**
   * construct
   *   Build this object from a set instance id
   *
   * @param $id The set instance ID
   * @todo don't know how best to build this
   */
  function __construct($type, $entity, $party = null) {
    // If the party isn't set, this is just an entity really, so we create 
    // A "temporary" instance of the wrapper that gets saved when we attach
    // it.
    $this->type = $type;
    $this->entity = $entity;
    $this->eid = $entity->{$this->getEntityIdKey()};
    
    // If the party is set we set the party and find the row in the data set table
    // to get the rest of the information
    if(isset($party)) {
      $this->party = $party;
      $this->pid = $party->pid;
      
      $query = db_select($this->getTableName(), 'ds');
      $query->fields('ds');
      $query->condition('pid', $this->pid, '=');
      $query->condition('eid', $this->eid, '=');
      $result = $query->execute()->fetchAssoc();
      
      // If there is no data set instance in the db we unset party and treat this like it
      // hasn't been attached yet;
      if(!$result) {
        unset($this->party);
        unset($this->pid);
      }
      else {
        $this->main = $result['main'];
        $this->hat = $result['hat'];
        $this->hat_main = $result['hat_main'];
        $this->id = $result['peid'];
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
    if(!isset($this->entity)) {
      $this->entity = entity_load($this->getEntityType(), $eid);
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
  public function attach($party) {
    $relation = array(
      'pid' => $party->pid,
      'eid' => $this->entity->{$this->getEntityIdKey()},
    );

    // WIP: chuck in defaults until I figure out what these do
    $relation += array(
      'main' => FALSE,
      'hat' => 'trilby',
      'hat_main' => FALSE,
    );

    // @todo: figure out when to insert and when to update! we probably need
    // an extra parameter for this. #fail :(
    drupal_write_record($this->getTableName(), $relation);
  }
  
  /**
   * detach
   *   detach this data from the party
   */
  public function detach() {
    db_delete($this->getTableName())
    ->condition('peid', $this->id)
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
  
  }
  
  /**
   * isMain
   *   is this the main instance of this set for this party
   *
   * @return
   *   True is the Set is the main one, False otherwise.
   */
  public function isMain() {
  
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
   * getTable
   */
  private function getTableName() {
    return 'crm_party_set_' . $this->type;
  }
}