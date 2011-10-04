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
