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
    '#title' => 'Create a Party when a User Registers?',
    '#default_value' => variable_get('party_user_create_on_register', FALSE),
  );

  $option = array();
  foreach (party_get_party_types() as $type => $def) {
    $options[$type] = $def['label'];
  }
  
  $form['register_party_type'] = array(
    '#type' => 'select',
    '#title' => 'Type of party to create when a User Registers',
    '#options' => $options,
    '#default_value' => variable_get('party_user_register_party_type', 'individual'),
  );    
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
    '#weight' => 40,
  );
  
  return $form;
}

function party_user_settings_form_submit($form, &$form_state) {
  variable_set('party_user_create_on_register', ($form_state['values']['create_on_register'] == 1));
  variable_set('party_user_register_party_type', $form_state['values']['register_party_type']);
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
    $batch['operations'][] = array('party_user_batch_user_sync', array($user, array('type' => 'individual')));
  }
  
  batch_set($batch);
  
  $form_state['redirect'] = 'admin/config/party/user/sync';
}