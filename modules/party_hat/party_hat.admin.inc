<?php
/**
 * @file
 * Contains admin page callbacks for the CRM party hats module.
 */

/**
 * Entity form for party hats.
 *
 * @param $hat
 *  A hat object.
 */
function party_hat_form($form, &$form_state, $hat, $op = 'edit') {
  $form['#hat'] = $hat;

  if (isset($hat->hid)) {
    $form['hid'] = array(
      '#type' => 'hidden',
      '#value' => $hat->hid,
    );
    drupal_set_title(t('Edit hat: @label', array(
      '@label' => $hat->label,
    )));
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
      'exists' => 'party_hat_hat_machine_name_exists',
      'source' => array('label'),
    ),
    '#disabled' => isset($hat->name),
  );

  field_attach_form('party_hat', $hat, $form, $form_state);

  $form['required'] = array(
    '#type' => 'checkbox',
    '#title' => t('Required'),
    '#default_value' => (isset($hat->required)) ? $hat->required : 0,
    '#description' => 'If enabled, all parties require this hat.',
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

  foreach (party_get_data_set_info() as $data_set_name => $data_set) {
    // Get the rule.
    if (isset($hat->name)) {
      $default = party_hat_get_data_set_rule($hat, $data_set_name);
      foreach ($default as $key => $var) {
        if ($var == 1) {
          $default[$key] = $key;
        }
      }
    }
    else {
      $default = array('has' => 0, 'multiple' => 0);
    }

    $form['data_set_rules'][$data_set_name] = array(
      '#type' => 'checkboxes',
      '#title' => $data_set['label'],
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
    $row[] = $form[$data_set]['#title'];
    foreach (array_keys($form[$data_set]['#options']) as $radio_key) {
      // Remove the checkbox label; it's shown in the table header.
      unset($form[$data_set][$radio_key]['#title']);
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
function party_hat_form_validate($form, &$form_state) {
  $pseudo_entity = (object) $form_state['values'];
  field_attach_form_validate('party_hat', $pseudo_entity, $form, $form_state);
}

/**
 * Form submission for the hat edit form.
 */
function party_hat_form_submit($form, &$form_state) {
  // Build the entity and save it.
  $hat = entity_ui_form_submit_build_entity($form, $form_state);

  // @todo. if we're going to have a helper function to set this, it should
  // actually help and do all this messy stuff.
  foreach ($form_state['values']['data_set_rules'] as $data_set_name => $values) {
    // convert to a 1/0 column. There must be a better way! It probably involves having two different checkboxes
    foreach ($values as $key => $var) {
      if ($key === $var) {
        $values[$key] = 1;
      }
    }
    party_hat_set_data_set_rule($hat, $data_set_name, $values);
  }

  party_hat_save($hat);

  field_attach_submit('party_hat', $hat, $form, $form_state);

  drupal_set_message(t("Party hat has been saved."));

  // If editing an existing hat, redirect to the hat admin page.
  if ($form_state['build_info']['args'][1] == 'edit') {
    $form_state['redirect'] = 'admin/community/hats';
  }
}

/**
 * Helper to check whether a machine name already exists.
 */
function party_hat_hat_machine_name_exists($value) {
  $exists = db_query_range('SELECT 1 FROM {party_hat} WHERE `name` = :name', 0, 1, array(':name' => $value))->fetchField();
  return $exists;
}
