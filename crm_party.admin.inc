<?php

/**
 * @file
 * Admin page callback file for the crm_party module.
 */
 
function crm_party_admin($callback_arg = '') {
  $op = (isset($_POST['op'])) ? $_POST['op'] : $callback_arg;

  switch ($op) {
    default: 
      //$build['party_filter_form'] = drupal_get_form('party_filter_form');
      $build['party_admin_form'] = drupal_get_form('party_admin_form');
  }
  
  return $build;
}

/**
 * Form builder; Parties administration page.
 *
 * @ingroup forms
 * @see party_admin_form_validate()
 * @see party_admin_form_submit()
 */
function party_admin_form() {
  // Header for the tableselect element.
  $header = array(
    'pid' => array('data' => t('Party id'), 'field' => 'cp.pid'),
    'label' => array('data' => t('Label'), 'field' => 'cp.label'),
    'type' => array('data' => t('Party type'), 'field' => 'cp.type'),
  );
  
  $query = db_select('crm_party','cp');
  $query->condition('cp.pid', 0, '<>');
  
  $count_query = clone $query;
  $count_query->addExpression('COUNT(cp.pid)');

  $query = $query->extend('PagerDefault')->extend('TableSort');
  $query
    ->fields('cp', array('pid', 'type', 'label'))
    ->limit(50)
    ->orderByHeader($header)
    ->setCountQuery($count_query);
  $result = $query->execute();
    
  // This gives us the bulk operations form element
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Update options'),
    '#attributes' => array('class' => array('container-inline')),
  );
  $options = array();
  //get party operations from any modules that implement hook_party_operations
  foreach (module_invoke_all('party_operations') as $operation => $array) {
    $options[$operation] = $array['label'];
  }
  $form['options']['operation'] = array(
    '#type' => 'select',
    '#title' => t('Operation'),
    '#title_display' => 'invisible',
    '#options' => $options,
    '#default_value' => 'merge',
  );
  $options = array();
  $form['options']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );

  // Builds the table list.
  $destination = drupal_get_destination();

  // Get the party info for the type labels.
  $entity_info = crm_party_entity_info();
  $party_entity_info = $entity_info['crm_party'];

  $parties = array();
  foreach ($result as $party) {
    $options[$party->pid] = array(
      'pid' => array(
        'data' => array(
          '#type' => 'link',
          '#title' => $party->pid,
          '#href' => 'party/' . $party->pid,
        ),
      ),
      'label' => array(
        'data' =>array(
          '#type' => 'link',
          '#title' => $party->label,
          '#href' => 'party/' . $party->pid,
        ),
      ),
      'type' => $party_entity_info['bundles'][$party->type]['label'],
    );
    //implement making this hookable
  }

  $form['parties'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No parties available.'),
  );
  $form['pager'] = array('#markup' => theme('pager'));

  return $form;
}

/**
 * Settings form for choosing the active party label plugin.
 */
function crm_party_settings_label_plugin_form($form, &$form_state) {
  // @todo: hook_help text for this page.

  // Get the label plugin.
  ctools_include('plugins');

  $setting_plugin_name = variable_get('crm_party_name_label_plugin', 'pid');
  $label_plugins = ctools_get_plugins('crm_party', 'party_name_label');

  // spoofed in extra item for testing.
  $label_plugins['foo'] = array(
    'name' => 'foo',
    'title' => 'Foo',
    'description' => 'Dummy item for testing multiple radio buttons. Don\t pick me!',
  );

  $form['label'] = array(
    '#type' => 'item',
    '#title' => t('Party name label'),
    '#description' => t('Choose which method to use to generate a label for each party.'),
  );

  foreach ($label_plugins as $plugin_name => $plugin) {
    $form['label'][$plugin_name] = array(
      '#type' => 'radio',
      // Strings from the plugin are already translated.
      '#title' => $plugin['title'],
      '#description' => $plugin['description'],
      '#return_value' => $plugin_name,
      '#default_value' => ($plugin_name == $setting_plugin_name),
      // The #parents property lets single radio buttons behave as a set.
      '#parents' => array('label'),
    );
  }

  $form['submit'] = array('#type' => 'submit', '#value' => t('Save'));

  return $form;
}

/**
 * Submit handler for the party label plugin settings form.
 */
function crm_party_settings_label_plugin_form_submit($form, &$form_state) {
  //dsm($form_state['values']);
  variable_set('crm_party_name_label_plugin', $form_state['values']['label']);
}
