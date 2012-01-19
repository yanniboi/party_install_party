<?php

/**
 * @file
 * Admin page callback file for the party module.
 */

/**
 * Mock up the Party admin interface - this is horrible: do it a better way.
 */
function party_info() {
  // @todo: probably some of this admin UI can be done by EntityAPI.

  $entity_info = entity_get_info('party');
  $types = $entity_info['bundles'];

  $rows = array();

  foreach ($types as $type => $info) {
    $name = check_plain($info['label']);
    $name .= ' <small>' . t('(Machine name: @type)', array('@type' => $type)) . '</small>';
    $name .= '<div class="description">' . check_plain($info['description']) . '</div>';
    $rows[] = array(
      $name,
      array(
        'data' => l(t('edit'), 'admin/community/party/' . $type),
      ),
      array(
        'data' => l(t('Manage fields'), 'admin/community/party/' . $type . '/fields'),
      ),
      array(
        'data' => l(t('Manage display'), 'admin/community/party/' . $type . '/display'),
      ),
    );
  }


  return array(
    'party_table' => array(
      '#theme' => 'table',
      '#header' => array(
        t('Party type'), array('data' => t('Operations'), 'colspan' => '3'),
      ),
      '#rows' => $rows,
    ),
  );
}

/**
 * The management page for each party type.
 */
function party_manage($type) {
  return "Welcome to the party type administration page for " . $type;
}

/**
 * Page callback for party admin list.
 *
 * @param $callback_arg
 *  @todo write me probably cribbed from node admin ;)
 */
function party_admin($callback_arg = '') {
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
  
  $query = db_select('party','cp');
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
  $entity_info = party_entity_info();
  $party_entity_info = $entity_info['party'];

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
 * Manage Data Sets Page
 *
 * @todo: Get links on this page to always redirect back to this page.
 */
function party_data_set_admin() {
  $data_sets = party_get_data_set_info();
  
  $header = array(
    0 => t('Data set name'),
    1 => array(
      'data' => t('Operations'),
      'colspan' => '6',
    ),
  );
  
  $rows = array();
  foreach ($data_sets as $key => $data_set) {
    $name = check_plain($data_set['label']);
    $name .= ' <small>' . t('(Machine name: @set-name)', array('@set-name' => $key)) . '</small>';
    $ops = array('edit', 'manage fields', 'manage display', 'clone', 'export', 'delete');
    
    $row = array($name);
    foreach ($ops as $op) {
      $row[] = array(
        'data' => isset($data_set['admin'][$op]) ? l($op, $data_set['admin'][$op], array('query' => array('destination' => 'admin/community/datasets'))) : '',
      );
    }
    
    $rows[] = $row;
  }
  
  $build['dataset_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('No data sets available'),
  );
  
  return $build;
}

/**
 * Settings form for choosing the active party label plugin.
 */
function party_settings_label_plugin_form($form, &$form_state) {
  // @todo: hook_help text for this page.

  // Get the label plugin.
  ctools_include('plugins');

  $setting_plugin_name = variable_get('party_name_label_plugin', 'pid');
  $label_plugins = ctools_get_plugins('party', 'party_name_label');

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
function party_settings_label_plugin_form_submit($form, &$form_state) {
  variable_set('party_name_label_plugin', $form_state['values']['label']);
  
  $form_state['redirect'] = 'admin/config/party/labels/' . $form_state['values']['label'];
}

/**
 * Get the Plugin settings form
 */
function party_settings_label_plugin_config_page($plugin_id) {
  // Get the label plugin.
  ctools_include('plugins');
  
  $label_plugin = ctools_get_plugins('party', 'party_name_label', $plugin_id);
  
  if (isset($label_plugin['options form callback'])) {
    return drupal_get_form($label_plugin['options form callback']);
  }
  else {
    drupal_goto('admin/config/party/labels');
  }
}

/**
 * Settings form for ordering the party pieces.
 */
function party_settings_pieces_order_form($form, &$form_state) {
  // Get the pieces. These come in with the stored weight setting already.
  $pieces = party_get_party_piece_info();

  $form['#tree'] = TRUE;
  foreach ($pieces as $path => $piece) {
    $form['pieces'][$path]['name'] = array('#markup' => check_plain($piece['title']));
    $form['pieces'][$path]['weight'] = array(
      '#type' => 'weight',
      '#title' => t('Weight for @title', array('@title' => $piece['title'])),
      '#title_display' => 'invisible',
      '#delta' => 10,
      '#default_value' => isset($piece['weight']) ? $piece['weight'] : 0,
    );
  }

  // Only make this form include a submit button and weight if more than one
  // piece exists.
  if (count($pieces) > 1) {
    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save'));
  }
  elseif (isset($piece)) {
    unset($form['pieces'][$path]['weight']);
  }

  return $form;
}

/**
 * Returns HTML for the vocabulary overview form as a sortable list of vocabularies.
 *
 * @param $variables
 *   An associative array containing:
 *   - form: A render element representing the form.
 *
 * @see taxonomy_overview_vocabularies()
 * @ingroup themeable
 */
function theme_party_settings_pieces_order_form($variables) {
  $form = $variables['form'];

  $rows = array();

  foreach (element_children($form['pieces']) as $key) {
    $piece = &$form['pieces'][$key];

    $row = array();
    $row[] = drupal_render($piece['name']);
    if (isset($piece['weight'])) {
      $piece['weight']['#attributes']['class'] = array('piece-weight');
      $row[] = drupal_render($piece['weight']);
    }
    $rows[] = array('data' => $row, 'class' => array('draggable'));
  }

  $header = array(t('Piece label'));
  if (isset($form['actions'])) {
    $header[] = t('Weight');
    drupal_add_tabledrag('crm-party-pieces-order', 'order', 'sibling', 'piece-weight');
  }
  return theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => 'crm-party-pieces-order'))
  ) . drupal_render_children($form);
}

/**
 * Submit handler for the party pieces order form.
 */
function party_settings_pieces_order_form_submit($form, &$form_state) {
  $settings = array();
  foreach (array_keys($form_state['values']['pieces']) as $path) {
    $settings[$path] = $form_state['values']['pieces'][$path]['weight'];
  }

  variable_set('party_name_pieces_weights', $settings);
  menu_rebuild();
}

/**
 * Menu callback of development information.
 */
function party_devel_info_page() {
  $info = party_get_data_set_info();
  $output = kprint_r($info, TRUE, t('Data set info'));
  $info = party_get_party_piece_info();
  $output .= kprint_r($info, TRUE, t('Party piece info'));
  return $output;
}
