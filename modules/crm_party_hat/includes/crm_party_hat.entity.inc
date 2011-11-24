<?php
/**
 * @file
 * Contains the controller classes for Party entities.
 */

/**
 * The API controller class for the Party Hat entity.
 */
class CRMPartyHatController extends EntityAPIController {
  
}
  
/**
 * The class used for hat entities.
 */
class CRMPartyHat extends Entity {  
  public function __construct($values = array()) {
    parent::__construct($values, 'crm_party_hat');
  }

  protected function defaultLabel() {
    return $this->label;
  }

  protected function defaultUri() {
    return array('path' => 'party-hat/' . $this->id);
  }   
}
