<?php

/**
 * @file
 * Hooks provided by the Party module.
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
 * @see party_access()
 */
function hook_party_access($op, $party = NULL, $attached_entity = NULL, $account = NULL) {

}
 
/**
 * Defines data sets to be used by parties.
 *
 * @return
 *  An array of sets where each key is the unique identifier of that "set type".
 *  - 'label': The human readable name of the data set.
 *  - 'entity type': The entity type this data set relates to parties.
 *  - 'entity bundle': (optional) The entity bundle that this data set restricts
 *    to. May be omitted to allow any bundle.
 *  - 'singleton': (optional) Whether this set's relationships only have one
 *    entity relating to a party. Default: FALSE.
 *  - 'form callback': (optional) This is the name of the form callback function.
 *    Returns the section of the form to do with this data set. See party_default_attached_entity_form()
 *  - 'module': (optional) The name of the module implementing this data set.
 *    This will be filled in automatically if not supplied.
 *  - 'admin': An array of admin paths for configuring and managing the piece
 *    - 'edit'
 *    - 'manage fields'
 *    - 'manage display'
 *    - 'delete'
 *    - 'create'
 *    - 'import'
 *    - 'clone'
 *    - 'export'
 *  - piece: (optional) Each set may define one party piece. The contents of
 *    this array should be the same as those returned by
 *    hook_party_party_piece_info(), with the addition of:
 *    - 'path': The menu path for the provided piece.
 *    - 'uses views': (optional) Indicates that the piece should be generated
 *      with a default view. Default to FALSE.
 *    - 'view name': (optional) @todo! write the code for this! ;)
 *      The machine name of the view to define in
 *      hook_views_default_views(). This allows multiple default views to exist.
 *      Defaults to 'party_attached_entities'.
 */
function hook_party_data_set_info() {
  $sets = array();
  
  // A user data set.
  $sets['user'] = array(
    'label' => t("User account"),
    'entity type' => 'user',
    'singleton' => TRUE, // There is only one user per party.
    'load callback' => "party_user_load_user",
    'form callback' => "party_user_form_user",
  );
  return $sets;
}

/**
 * Defines party pieces, that is, components of the party display.
 *
 * @return
 *  An array of display pieces (similar to hook_menu) where each key is the
 *  unique Id of the display.
 *   - 'label': The human readable name of the party piece.
 *   - 'display callback': The name of the display function. This is always
 *     given $party, $instance, $title, $arguments. This should return a
 *     renderable array.
 *   - 'display callback arguments': Any extra arguments to supply to the
 *     display callback.
 *   - 'data set': (optional) The data set this is associated with. (If this is
 *     set, edit and add links will appear around the piece linking to the forms
 *     laid out in data_set_info);
 *   - 'nesting': True if the party piece can have children.
 */
function hook_party_party_piece_info() {
  $pieces = array();
  $pieces['user_username'] = array(
    'label' => "Username only",
    'display callback' => "party_user_display_user",
    'display arguments' => array('username'),
    'data set' => 'user',
  );
  $pieces['user_fullaccount'] = array(
    'label' => "Username only",
    'display callback' => "party_user_display_user",
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
 *  below 'party/%party/view'.
 *  A number of defaults will be added by party_menu().
 */
function hook_party_party_pieces() {
  return array(
    'party' => array(
      'title' => 'View',
      'page callback' => 'party_page_view',
      'page arguments' => array(1),
      'file' => 'party.pages.inc',
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
      'callback' => 'party_party_operations_merge',
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
      'callback' => 'party_party_admin_columns',
    ),
  );
}

/**
 * Attach an entity to a party.
 *
 * @param $party
 *  The party to assign the entity to.
 * @param $entity
 *  The entity to relate to.
 * @param $data_set
 *  The name of the data set.
 *  DX WTF: can we sniff this out given the entity type and the entity object? Yes, but we need to be able to get the entity type from the object.
 */
function party_attach_entity($party, $entity, $data_set_name) {
  $attached_entity = party_get_crm_controller($data_set_name);
  $attached_entity->setAttachedEntity($entity);  
  $attached_entity->attach($party);
}

/**
 * Detach an entity from a party according to a given data set.
 *
 * @param $party
 *  The party to detach the entity from.
 * @param $entity
 *  The entity to detach. This may also be just the entity id.
 * @param $data_set
 *  The name of the data set.
 *  DX WTF: can we sniff this out given the entity type and the entity object?
 */
function party_detach_entity($party, $entity, $data_set_name) {
  /* To Test */
  $attached_entity = party_get_crm_controller($data_set_name);
  $attached_entity->setParty($party);
  $attached_entity->setAttachedEntity($entity);  
  $attached_entity->detach();
}


