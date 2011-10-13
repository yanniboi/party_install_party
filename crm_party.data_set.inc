<?php

/**
 * The standard data set class
 */
abstract class CRMPartyDataSet {
  
  /**
   * abstract $entity_type
   *   The entity type this Data Set Wraps around
   */
  static protected $entity_type = "";
  
  /**
   * abstract $entity_id_col
   *   The entity type this Data Set Wraps around
   */
  static protected $entity_id_column = "";
  
  
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
   */
  function __construct($id) {
  
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
      $this->entity = entity_load($this->entity_type, $eid);
    }
    
    return $this->entity() 
  }
  
  /**
   * attach
   *   attach this data set to a party
   * 
   * @param $party
   *   The party object we're attaching to.
   */
  public function attach($party) {
  
  }
  
  /**
   * detach
   *   detach this data from the party
   */
  public function detach() {
  
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
   * @group Static Helper Functions
   */
   
  /**
   * getAllDataSets
   *   Get all the data sets connected to a party of a particular type
   * 
   * @param Party $party The party we're getting the data sets of;
   */
  public abstract static function getAllDataSets($party) {
  }
}