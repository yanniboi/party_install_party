<?php

/**
 * @file
 * Hooks provided by the CRM Party module.
 */

/**
 * @addtogroup hooks
 * @{
 */
 
/**
 * Defines data sets to be used by parties
 *
 * @return
 *  And array of sets (similar to hook_menu()) where each key is the unique
 *  identifier of that "set type".
 *   - "label" the human readable name of the data set
 *   - "load callback" the name of the load function. This always gets given the $party object, set_type and set_id
 *   - "load callback arguements" any extra args to supply to the load callback //needed?!
 *   - "form callback" the name of the form function. This function should return a set of form fields (but not 
 *       the submit button). It gets $party, $set_type, $set_id, $form and $form_state.
 *   - "module" the name of the module implementing this data set
 */
function hook_party_data_set_info() {
  $sets = array();
  
  //A user data set.
  $sets['user'] = array(
    'label' => "User Account",
    'load callback' => "crm_user_load_user",
    'form callback' => "crm_user_form_user",
  );
  return $sets;
}

/**
 * Defines party pieces, that is, components of the party display.
 *
 * @return
 *  An array of display pieces (similar to hook_menu) where each key is the unique Id of the display
 *   - "label" The human readable name of the party piece
 *   - "display callback" The name of the display function. This is always given$ party_id, 
 *       $delta, $piece_id, $set_ids. This should return a renderable array.
 *   - "display callback arguments" any extra arguments to supply to the display callback
 *   - "data set" (optional) the data set this is associated with. (If this is set, edit and add links
 *       will appear around the piece linking to the forms laid out in data_set_info);
 *   - "nesting" true if the party piece can have children
 */
function hook_party_party_piece_info() {
  $pieces = array();
  $pieces['user_username'] = array(
    'label' => "Username only",
    'display callback' => "crm_user_display_user",
    'display arguments' => array('username'),
    'data set' => 'user',
  );
  $pieces['user_fullaccount'] = array(
    'label' => "Username only",
    'display callback' => "crm_user_display_user",
    'display arguments' => array('full account'),
    'data set' => 'user',
  );
  return $pieces;
}

/**
 * Defines party pieces, that is, components of the party display.
 *
 * @return
 *  An array of items suitable for hook_menu(), where each key is the subpath
 *  below 'party/%crm_party/view'.
 *  A number of defaults will be added by crm_party_menu().
 */
function hook_crm_party_party_pieces() {
  return array(
    'party' => array(
      'title' => 'View',
      'page callback' => 'crm_party_page_view',
      'page arguments' => array(1),
      'file' => 'crm_party.pages.inc',
      'access arguments' => array('view contacts'),
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => -10,
    ),
  );
}

/**
 * Add mass party operations for the Community admin page
 *
 * This hook allows modules to inject custom operations into the Community admin
 * page. The callback specified receives one argument: an array of the selected
 * party objects.
 *
 * @return
 *   An array of operations with keys:
 *   - "label": Required. The label for the drop down menu.
 *   - "callback": Required. The function that processes the operation.
 *   - "callback arguments": Optional. Any extra arguments to be sent to the
 *    callback function.
 */
function hook_party_operations() {
  $operations = array(
    'merge' => array(
      'label' => t('Merge parties'),
      'callback' => 'crm_party_party_operations_merge',
    ),
  );
  return $operations;
}

/**
 * Add a column to the table on the Community admin page.
 *
 * This hook allows modules who change the party implementations to send their
 * data to the Community admin page.
 *
 * @return
 *   An array of columns with keys:
 *   - "title": Required. The name of the column.
 *   - "callback": Required. A function that returns the value of the column. This is sent a party object
 *   - "callback arguments": Optional: Any extra arguments to be sent.
 *   - "field": Optional: I don't know what this does.
 *
 */
function hook_party_admin_columns_info() {
  $columns = array(
    'pid' => array(
      'label' => t('Party Id'),
      'field' => 'cp.pid',
      'callback' => 'crm_party_party_admin_columns',
    ),
  );
}
