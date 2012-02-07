<?php

/**
 * @file
 * Contact type editing UI.
 */

/**
 * UI controller.
 */
class ContactTypeUIController extends EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
		$items[$this->path]['description'] = 'Manage contact entity types, including adding
		and removing fields and the display of fields.';
    return $items;
  }
}

/**
 * Generates the contact type editing form.
 */
function contact_type_form($form, &$form_state, $contact_type, $op = 'edit') {

  if ($op == 'clone') {
    $contact_type->label .= ' (cloned)';
    $contact_type->type = '';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $contact_type->label,
    '#description' => t('The human-readable name of this contact type.'),
    '#required' => TRUE,
    '#size' => 30,
  );
  // Machine-readable type name.
  $form['type'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($contact_type->type) ? $contact_type->type : '',
    '#maxlength' => 32,
//    '#disabled' => $contact_type->isLocked() && $op != 'clone',
    '#machine_name' => array(
      'exists' => 'contact_get_types',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this contact type. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['data']['#tree'] = TRUE;
  $form['data']['sample_data'] = array(
    '#type' => 'checkbox',
    '#title' => t('An interesting contact switch'),
    '#default_value' => !empty($contact_type->data['sample_data']),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save contact type'),
    '#weight' => 40,
  );

  //Locking not supported yet
  /*if (!$contact_type->isLocked() && $op != 'add') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete contact type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('contact_type_form_submit_delete')
    );
  }*/
  return $form;
}

/**
 * Form API submit callback for the type form.
 */
function contact_type_form_submit(&$form, &$form_state) {
  $contact_type = entity_ui_form_submit_build_entity($form, $form_state);
  $contact_type->save();
  $form_state['redirect'] = 'admin/structure/contact_types';
}

/**
 * Form API submit callback for the delete button.
 */
function contact_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/contact_types/manage/' . $form_state['contact_type']->type . '/delete';
}
