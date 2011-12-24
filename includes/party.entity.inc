<?php

/**
 * @file
 * Contains the controller classes for Party entities.
 */

/**
 * The API controller class for the Party entity.
 */
class PartyController extends EntityAPIController {
  
  /**
   * Save a party.
   *
   * The party's label is determined by the currently active name label plugin.
   */
  public function save($party) {
    // Set the label on the party.
    $this->setLabel($party);

    if (isset($party->pid)) {
      drupal_write_record('party', $party, 'pid');
      field_attach_update('party', $party);
      module_invoke_all('entity_update', $party, 'party');
    }
    else {
      drupal_write_record('party', $party);
      field_attach_insert('party', $party);
      module_invoke_all('entity_insert', $party, 'party');
    }
    return $party;
  }

  /**
   * Set the label on a party object using the current label plugin.
   *
   * @param $crmParty
   *  A party object.
   */
  public function setLabel($crmParty) {
    // Get the label plugin.
    ctools_include('plugins');
    $label_plugin_name = variable_get('party_name_label_plugin', 'pid');
    $label_plugin = ctools_get_plugins('party', 'party_name_label', $label_plugin_name);

    // Generate the party label from the plugin.
    $label_callback = $label_plugin['label callback'];
    $crmParty->label = $label_callback($crmParty);
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
class Party extends Entity {
  
  public function __construct($values = array()) {
    parent::__construct($values, 'party');
  }

  protected function defaultLabel() {
    return $this->name;
  }

  protected function defaultUri() {
    return array('path' => 'party/' . $this->id);
  }   
}
