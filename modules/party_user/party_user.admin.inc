<?php
/**
 * @file Provide config forms for views
 */

/**
 * User Integration settings Form
 */
function party_user_settings_form($form, &$form_state) {
  $form['create_on_register'] = array(
    '#type' => 'checkbox',
    '#title' => 'Create a party when a user registers.',
    '#default_value' => variable_get('party_user_create_on_register', FALSE),
  );

  $options = array();
  foreach (party_get_data_set_info() as $data_set_name => $def) {
    if ($data_set_name != 'user') {
      $options[$data_set_name] = $def['label'];
    }
  }

  $form['registration_data_sets'] = array(
    '#type' => 'checkboxes',
    '#options' => $options,
    '#title' => 'Data sets to show on user registration form',
    '#default_value' => variable_get('party_user_registration_data_sets', array()),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save configuration',
    '#weight' => 40,
  );

  return $form;
}

function party_user_settings_form_submit($form, &$form_state) {
  variable_set('party_user_create_on_register', ($form_state['values']['create_on_register'] == 1));

  $registration_data_sets = $form_state['values']['registration_data_sets'];
  foreach ($registration_data_sets as $key => $var) {
    if ($var === 0) {
      unset($registration_data_sets[$key]);
    }
  }
  $registration_data_sets = array_keys($registration_data_sets);
  variable_set('party_user_registration_data_sets', $registration_data_sets);
}

/**
 * User Sync Form
 *
 * @todo: Optionally use the rule event to sync
 * @todo: Limit by role
 * @todo: Choose which Party Types to create
 */
function party_user_sync_form($form, &$form_state) {
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Synchronise Users',
  );

  return $form;
}

function party_user_sync_form_submit($form, &$form_state) {
  // Get all Users
  $users = db_select('users', 'u')
    ->fields('u', array('uid', 'name'))
    ->condition('uid', 0, '<>')
    ->execute()
    ->fetchAllAssoc('uid', PDO::FETCH_ASSOC);

  $batch = array(
    'operations' => array(),
    'finished' => 'party_user_sync_finished',
    'title' => t("Processing User Sync"),
    'init_message' => t('User Sync is starting'),
    'progress_message' => t('Processed @current out of @total'),
    'error_message' => t('User Sync Encountered an Error'),
    'file' => drupal_get_path('module', 'party_user') . '/party_user.batch.inc',
  );

  foreach ($users as $user) {
    $batch['operations'][] = array('party_user_batch_user_sync', array($user, array()));
  }

  batch_set($batch);

  $form_state['redirect'] = 'admin/config/party/user/sync';
}