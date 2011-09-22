<?php

/**
 * @file
 * Contains the controller classes for Party entities.
 */

/**
 * The API controller class for the Party entity.
 */
class CRMPartyController extends EntityAPIController {
  
  /**
   * Save a party.
   */
  public function save($crmParty) {
    if (isset($crmParty->pid)) {
      drupal_write_record('crm_party', $crmParty, 'pid');
      field_attach_update('crm_party', $crmParty);
      module_invoke_all('entity_update', 'crm_party', $crmParty);
    } 
    else {
      drupal_write_record('crm_party', $crmParty);
      field_attach_insert('crm_party', $crmParty);
      module_invoke_all('entity_insert', 'crm_party', $crmParty);
    }
    return $crmParty;
  }
  
  
  /**
   * Overriding the buildContent function to add entity specific fields.
   */
  public function buildContent($entity, $view_mode = 'full', $langcode = NULL, $content = array()) {
    $content = parent::buildContent($entity, $view_mode, $langcode, $content);
    return $content;
  }
}
  
/**
 * The class used for model entities.
 */
class CRMParty extends Entity {
  
  public function __construct($values = array()) {
    parent::__construct($values, 'crm_party');
  }

  protected function defaultLabel() {
    return $this->name;
  }

  protected function defaultUri() {
    return array('path' => 'party/' . $this->id);
  }   
}
