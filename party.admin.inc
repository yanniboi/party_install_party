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
function party_type_form($form, &$form_state, $type) {
  $required_data_sets = variable_get('party_' . $type . '_required_data', array());
  $data_sets = party_get_data_set_info();
  foreach ($data_sets as $data_set_name => $def) {
    $options[$data_set_name] = $def['label'];
  }

  $form['type'] = array(
    '#type' => 'value',
    '#value' => $type,
  );

  $form['required_data_sets'] = array(
    '#type' => 'checkboxes',
    '#options' => $options,
    '#title' => 'Required Data Sets',
    '#default_value' => $required_data_sets,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
    '#weight' => 40,
  );

  return $form;
}

function party_type_form_submit($form, &$form_state) {
  $type = $form_state['values']['type'];
  $required_data_sets = $form_state['values']['required_data_sets'];
  foreach ($required_data_sets as $key => $var) {
    if ($var === 0) {
      unset($required_data_sets[$key]);
    }
  }
  $required_data_sets = array_keys($required_data_sets);
  variable_set('party_' . $type . '_required_data', $form_state['values']['required_data_sets']);
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
function party_settings_label_plugins_form($form, &$form_state) {
  // @todo: hook_help text for this page.
  $label_plugins = party_settings_get_party_label_plugins();

  $form['#tree'] = TRUE;
  foreach ($label_plugins as $path => $label_plugin) {
    $form['label_plugins'][$path]['name'] = array(
      '#markup' => check_plain($label_plugin['title'])
    );
    $form['label_plugins'][$path]['weight'] = array(
      '#type' => 'weight',
      '#title' => t('Weight for @title', array('@title' => $label_plugin['title'])),
      '#title_display' => 'invisible',
      '#delta' => 10,
      '#default_value' => isset($label_plugin['weight']) ? $label_plugin['weight'] : 0,
    );

    // Generate the edit link if the 'options_form_callback' is set.
    if (isset($label_plugin['options form callback'])) {
      $link = l('Edit', 'admin/config/party/labels/' . $path);
    }
    else {
      $link = '';
    }
    $form['label_plugins'][$path]['edit_link'] = array(
        '#markup' => $link,
    );
  }

  // Include a submit button and weight if more than one label plugin exists.
  if (count($label_plugins) > 1) {
    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save'));
  }
  elseif (isset($label_plugin)) {
    unset($form['label_plugins'][$path]['weight']);
  }

  return $form;
}

/**
 * Gets the available party label plugins and adds their weight from config.
 *
 * Weights are set at admin/config/party/labels.
 *
 * @return
 *  An array of party label plugins sorted by weight.
 *
 */
function party_settings_get_party_label_plugins() {
  // Get an array of the available label plugins.
  ctools_include('plugins');
  $label_plugins = ctools_get_plugins('party', 'party_name_label');

  // Add in weights saved previously
  $weights = variable_get('party_label_plugins', array());
  foreach ($label_plugins as $path => $label_plugin) {
     if (isset($weights[$path])) {
       $label_plugins[$path]['weight'] = $weights[$path]['weight'];
     }
  }

  // Sort label plugins array by weight for hook_menu() to figure out the default tab
  // and the admin UI to show them in the right order.
  // drupal_sort_weight() treats a missing weight key as a 0.
  uasort($label_plugins, 'drupal_sort_weight');
  return $label_plugins;
}

/**
 * Submit handler for the party label plugin settings form.
 */
function party_settings_label_plugins_form_submit($form, &$form_state) {
  variable_set('party_label_plugins', $form_state['values']['label_plugins']);
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
function theme_party_settings_label_plugins_form($variables) {
  $form = $variables['form'];

  $rows = array();

  foreach (element_children($form['label_plugins']) as $key) {
    $label_plugin = &$form['label_plugins'][$key];
    $row = array();
    $row[] = drupal_render($label_plugin['name']);
    $row[] = drupal_render($label_plugin['edit_link']);
    if (isset($label_plugin['weight'])) {
      $label_plugin['weight']['#attributes']['class'] = array('label-plugin-weight');
      $row[] = drupal_render($label_plugin['weight']);
    }
    $rows[] = array('data' => $row, 'class' => array('draggable'));
  }

  $header = array(t('Label Plugins'),t(''));
  if (isset($form['actions'])) {
    $header[] = t('Weight');
    drupal_add_tabledrag('crm-party-label-plugins', 'order', 'sibling', 'label-plugin-weight');
  }

  $output = theme('table', array(
      'header' => $header,
      'rows' => $rows,
      'attributes' => array('id' => 'crm-party-label-plugins'))
  ) . drupal_render_children($form);

  return $output;
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

  // Include a submit button and weight if more than one piece exists.
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
