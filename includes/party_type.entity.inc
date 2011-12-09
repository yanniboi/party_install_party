<?php

/**
 * @file
 * Party type editing UI.
 */

// @todo: move non-class functions elsewhere; then this file no longer needs
// to be included as it can autoload.

/**
 * The Controller for Model entities
 */
class PartyTypeController extends EntityAPIControllerExportable {
  public function __construct($entityType) {
    parent::__construct($entityType);
  }
  
  /**
   * Create a model type - we first set up the values that are specific
   * to our model type schema but then also go through the EntityAPIController
   * function.
   * 
   * @param $type
   *   The machine-readable type of the model.
   *
   * @return
   *   A model type object with all default fields initialized.
   */
  public function create(array $values = array()) {
    // Add values that are specific to our Model
    $values += array( 
      'id' => '',
      'is_new' => TRUE,
      'data' => '',
    );
    $party_type = parent::create($values);
    return $party_type;
  }

} 
 
/**
 * The class used for model type entities
 */
class PartyType extends Entity {
  
  public $type;
  public $label;
  
  public function __construct($values = array()) {
    parent::__construct($values, 'party_type');
  }
  
} 
 
/**
 * UI controller.
 */
class PartyTypeUIController extends EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
		$items[$this->path]['description'] = 'Manage Party entity types, including adding
		and removing fields and the display of fields.';
    return $items;
  }
}

/**
 * Generates the party type editing form.
 */
function party_type_form($form, &$form_state, $party_type, $op = 'edit') {

  if ($op == 'clone') {
    $party_type->label .= ' (cloned)';
    $party_type->type = '';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $party_type->label,
    '#description' => t('The human-readable name of this party type.'),
    '#required' => TRUE,
    '#size' => 30,
  );
  // Machine-readable type name.
  $form['type'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($party_type->type) ? $party_type->type : '',
    '#maxlength' => 32,
//    '#disabled' => $party_type->isLocked() && $op != 'clone',
    '#machine_name' => array(
      'exists' => 'party_get_types',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this party type. It must only contain lowercase letters, numbers, and underscores.'),
  );

  
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Party type'),
    '#weight' => 40,
  );

  //Locking not supported yet
  /*if (!$party_type->isLocked() && $op != 'add') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete party type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('party_type_form_submit_delete')
    );
  }*/
  return $form;
}

/**
 * Form API submit callback for the type form.
 */
function party_type_form_submit(&$form, &$form_state) {
  $party_type = entity_ui_form_submit_build_entity($form, $form_state);
  $party_type->save();
  $form_state['redirect'] = 'admin/community/structure/party_types';
}

/**
 * Form API submit callback for the delete button.
 */
function party_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/community/structure/party_types/manage/' . $form_state['party_type']->type . '/delete';
}