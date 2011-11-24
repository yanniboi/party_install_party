<?php
/**
 * @file
 * Contains admin page callbacks for the CRM party hats module.
 */

/**
 * Page callback for the hats list page.
 */
function crm_party_hat_manage() {
  $result = db_select('crm_party_hats', 'ph')
    ->fields('ph')
    ->execute()
    ->fetchAllAssoc('hid');
  
  $header = array(t('Hat'), array(
    'data' => t('Operations'),
    'colspan' => '2',
  ));
  $rows = array();
  foreach ($result as $hat) {
    $row = array( t('@label <small>(Machine name: @name)</small>', array(
      '@label' => $hat->label,
      '@name' => $hat->name,
    )));
    $row[] = array('data' => l(t('edit'), 'admin/community/hats/' . $hat->name . '/edit'));
    $row[] = array('data' => l(t('delete'), 'admin/community/hats/' . $hat->name . '/delete'));
    
    $rows[] = $row;
  }
  
  $build['hat_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('No hats have been defined. <a href="@link">Add a hat</a>', array('@link' => url('admin/community/hats/add'))),
  );
  
  return $build;
}

/**
 * Page callback for adding a new hat.
 */
function crm_party_hat_add() {
  $hat = new stdClass();
  return drupal_get_form('crm_party_hat_edit_form', $hat); 
}

/**
 * Form to add/edit a hat.
 *
 * @param $hat
 *  A hat object.
 */
function crm_party_hat_edit_form($form, &$form_state, $hat) {
  $form['#hat'] = $hat;
  
  if (isset($hat->hid)) {
    $form['hid'] = array(
      '#type' => 'hidden',
      '#value' => $hat->hid,
    );
    // @todo missing t().
    drupal_set_title('Edit hat: ' . $hat->label);
  }
  
  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Hat name'),
    '#required' => TRUE,
    '#default_value' => isset($hat->label) ? $hat->label : '',
  );
  
  $form['name'] = array(
    '#type' => 'machine_name',
    '#title' => t('Hat machine name'),
    '#default_value' => isset($hat->name) ? $hat->name : '',
    '#description' => t('A unique, machine readable name for the hat'),
    '#machine_name' => array(
      'exists' => 'crm_party_hat_hat_machine_name_exists',
      'source' => array('label'),
    ),
    '#disabled' => isset($hat->name),
  );
  
  field_attach_form('crm_party_hat', $hat, $form, $form_state);
  
  $form['required'] = array(
    '#type' => 'checkbox',
    '#title' => t('Required'),
    '#default_value' => (isset($hat->required)) ? $hat->required : 0,
    '#description' => 'Whether (individual) parties require this hat.',
    '#weight' => 1,
  );
  
  $form['data_set_rules'] = array(
    '#type' => 'fieldset',
    '#title' => t('Data rules'),
    '#description' => 'Set rules for which data set these hats give.',
    '#weight' => 3,
    '#tree' => TRUE,
    '#theme' => 'crm_hat_data_set_rules_form',
  );
  
  foreach (crm_party_get_data_set_info() as $type => $set) {
    // Get the rule.
    if (isset($hat->name)) {
      $default = crm_party_hat_get_data_set_rule($hat->name, $type);
      foreach($default as $key => $var) {
        if($var == 1) {
          $default[$key] = $key;
        }
      }
    }
    else {
      $default = array('has' => 0, 'multiple' => 0);
    }
    
    $form['data_set_rules'][$type] = array(
      '#type' => 'checkboxes',
      '#title' => t($set['label']),
      '#default_value' => $default,
      '#options' => array(
        // We have to use different keys to what the access hook expects for
        // FormAPI radio values to work.
        'has'   => t('Has'),
        'multiple'  => t('Can have Multiple'),
      ),
    );      
  }

  $form['data_set_access'] = array(
    '#type' => 'fieldset',
    '#title' => t('Data Access'),
    '#description' => 'Set rules for who can see data of a party with this hat',
    '#weight' => 5,
    '#tree' => TRUE,
  );
  
  $form['data_set_access']['placeholder'] = array(
    '#markup' => '<i>This is a placeholder, in future hats will be able to define who can see the data of the party they\'re attached too</i>',
  );
  
  $form['party_piece_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Party piece settings'),
    '#weight' => 10,
    '#tree' => true,
  );
  
  $form['party_piece_settings']['placeholder'] = array(
    '#markup' => '<i>This is a placeholder, in future hats will be able to define party pieces that can have views & data sets inside them.</i>',
  );
  
  $form['associated_roles'] = array(
    '#type' => 'fieldset',
    '#title' => t('Associated roles'),
    '#weight' => 20,
    '#tree' => true,
  );
  
  $form['associated_roles']['placeholder'] = array(
    '#markup' => '<i>This is a placeholder, in future hats will be able to define associated roles that get added to the User when this hat is assigned to the party</i>',
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 99,
  );

  return $form;  
}

/**
 * Theme function for the data set rules form fieldset.
 */
function theme_crm_hat_data_set_rules_form($variables) {
  $form = $variables['form'];

  $output = '';
  $rows = array();
  foreach (element_children($form) as $data_set) {
    $row = array();
    $row[] = $data_set;
    foreach (array_keys($form[$data_set]['#options']) as $radio_key) {
      // @todo: move label to headers.
      $row[] = drupal_render($form[$data_set][$radio_key]);
    }

    $rows[] = $row;
  }

  $header = array_merge(array(t('Data set')), $form[$data_set]['#options']);
  $output .= theme('table', array('rows' => $rows, 'header' => $header));

  return $output;
}

/**
 * Form validation for the hat edit form.
 */
function crm_party_hat_edit_form_validate($form, &$form_state) {
  $submission = (object) $form_state['values'];
  field_attach_form_validate('crm_party_hat', $submission, $form, $form_state);
}

/**
 * Form submission for the hat edit form.
 */
function crm_party_hat_edit_form_submit($form, &$form_state) {
  $hat = new CRMPartyHat();
  $hat->name = $form_state['values']['name'];
  $hat->label = $form_state['values']['label'];
  if(isset($form_state['values']['hid'])) {
    $hat->hid = $form_state['values']['hid'];
  }
  $hat->required = $form_state['values']['required'];
  crm_party_hat_save($hat);
  field_attach_submit('crm_party_hat', $hat, $form, $form_state); 
  
  dsm($form_state['values']['data_set_rules']);
  // @todo: fix this with the new values.
  
  foreach ($form_state['values']['data_set_rules'] as $type => $values) {
    // convert to a 1/0 column. There must be a better way! It probably involves having two different checkboxes
    foreach($values as $key => $var) {
      if($key === $var) {
        $values[$key] = 1;
      }
    }
    crm_party_hat_set_data_set_rule($hat->name, $type, $values);
  }
  
  //$form_state['redirect'] = 'admin/community/hats';  
}

/**
 * Helper to check whether a machine name already exists.
 */
function crm_party_hat_hat_machine_name_exists($value) {
  $exists = db_query_range('SELECT 1 FROM {crm_party_hats} WHERE `name` = :name', 0, 1, array(':name' => $value))->fetchField();
  return $exists;
}

/**
 * Confirm deletion form for hat.
 */
function crm_party_hat_delete_form($form, &$form_state, $hat) {
  $form['hat_id'] = array(
    '#type' => 'hidden',
    '#value' => $hat->hid,
  );
  
  $form['description'] = array(
    '#markup' => 'Are you shure you want to delete ' . $hat->label . '?',
  );
  
  $form['actions'] = array(
    '#type' => 'actions',
  );
  
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Delete',
  );
  
  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => 'Cancel',
    '#href' => 'admin/community/hats',
  );

  return $form;  
}

/**
 * Submit handler for the delete confirmation form.
 */
function crm_party_hat_delete_form_submit($form, &$form_state) {
  entity_delete('crm_party_hat', $form_state['values']['hat_id']);
  $form_state['redirect'] = 'admin/community/hats';
}
 