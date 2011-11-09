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
 * Control access to parties and their attachments.
 *
 * Modules may implement this hook if they want to have a say in whether or not
 * a given user has access to perform a given operation on a profile.
 *
 * @param $op
 *   The operation being performed.
 * @param $party
 *   A party to check access for.
 * @param $attached_entity
 *   (optional) An attached entity to check access for. If nothing is given,
 *   access for just the party itself is determined.
 * @param $account
 *   (optional) The user to check for. If no account is passed, access is
 *   determined for the global user.
 *
 * @return boolean
 *   Return TRUE to grant access, FALSE to explicitly deny access. Return NULL
 *   or nothing to not affect the operation.
 *   Access is granted as soon as a module grants access and no one denies
 *   access. Thus if no module explicitly grants access, access will be denied.
 *
 * @see crm_party_access()
 */
function hook_crm_party_access($op, $party = NULL, $attached_entity = NULL, $account = NULL) {

}
 
/**
 * Defines data sets to be used by parties.
 *
 * Beware of making entity queries from within this hook, as this is called from
 * within crm_party's hook_schema().
 * See http://drupal.org/node/1307506 for background.
 *
 * @return
 *  An array of sets where each key is the unique identifier of that "set type".
 *  - 'label': The human readable name of the data set.
 *  - 'entity type': The entity type this data set relates to parties.
 *  - 'entity bundle': (optional) The entity bundle that this data set restricts
 *    to. May be omitted to allow any bundle.
 *  - 'singleton': (optional) Whether this set's relationships only have one
 *    entity relating to a party. Default: FALSE.
 *  - 'load callback': The name of the load function. This always gets given
 *    the $party object, set_type, and set_id.
 *  - 'load callback arguments': Any extra args to supply to the load callback
 *    //needed?!
 *  - 'form callback': The name of the form function. This function should
 *    return a set of form fields (but not the submit button).
 *    It gets $party, $set_type, $set_id, $form and $form_state.
 *  - 'module': (optional) The name of the module implementing this data set.
 *    This will be filled in automatically if not supplied.
 *  - piece: (optional) Each set may define one party piece. The contents of
 *    this array should be the same as those returned by
 *    hook_party_party_piece_info(), with the addition of:
*     - 'path': The menu path for the provided piece.
 *    - 'uses views': (optional) Indicates that the piece should be generated
 *      with a default view. Default to FALSE.
 *    - 'view name': (optional) @todo! write the code for this! ;)
 *      The machine name of the view to define in
 *      hook_views_default_views(). This allows multiple default views to exist.
 */
function hook_crm_party_data_set_info() {
  $sets = array();
  
  // A user data set.
  $sets['user'] = array(
    'label' => t("User account"),
    'entity type' => 'user',
    'singleton' => TRUE,
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
 *   - "display callback" The name of the display function. This is always given $party, 
 *       $instance, $title, $arguments. This should return a renderable array.
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
