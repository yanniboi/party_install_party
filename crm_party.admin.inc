<?php

/**
 * @file
 * Admin page callback file for the crm_party module.
 */

/**
 * Mock up the CRM Party admin interface - this is horrible: do it a better way.
 */
function crm_party_info() {
  // @todo: load bundles from entity info.
  // @todo: probably some of this admin UI can be done by EntityAPI.
  $types = array(
    'individual' => array(
      'label' => t('Individual Party'),
      'description' => 'Parties representing individual people',
      'admin' => array(
        'path' => 'admin/community/party/manage/individual',
        'access arguments' => array('administer parties'),
      ),
    ),
    'organisation' => array(
      'label' => t('Organisation Party'),
      'description' => 'Parties representing organisations or groups of people',
      'admin' => array(
        'path' => 'admin/community/party/manage/organisation',
        'access arguments' => array('administer parties'),
      ),
    ),
  );

  $rows = array();

  foreach ($types as $type => $info) {
    $rows[] = array(
      $info['label'] . " <small>(Machine name: " . $type . ")</small><div class=\"description\">" . $info['description'] . "</div>",
      array(
        'data' => l(t('edit'), 'admin/community/party/manage/' . $type),
      ),
      array(
        'data' => l(t('Manage Fields'), 'admin/community/party/manage/' . $type . '/fields'),
      ),
      array(
        'data' => l(t('Manage Fields'), 'admin/community/party/manage/' . $type . '/display'),
      ),
      array(
        'data' => '', //for when we implement custom party types
      ),
    );
  }


  return array(
    'party_table' => array(
      '#theme' => 'table',
      '#header' => array(
        t('Party Type'), array('data' => t('Operations'), 'colspan' => '4'),
      ),
      '#rows' => $rows,
    ),
  );
}

/**
 * The management page for each party type.
 */
function crm_party_manage($type) {
  return "Welcome to the party type administration page for " . $type;
}

/**
 * Page callback for party admin list.
 *
 * @param $callback_arg
 *  @todo write me probably cribbed from node admin ;)
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
 * Page callback for the hats list page
 */
function crm_party_hat_manage() {
  $build['actions'] = array(
    '#markup' => l("Add a hat", 'admin/community/hats/add'),
  );
  
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
    $row = array( t(':label <small>:name</small>', array(':label' => $hat->label, ':name' => $hat->name)));
    $row[] = array( 'data' => l(t('edit'), 'admin/community/hats/' . $hat->name . '/edit'));
    $row[] = array( 'data' => l(t('delete'), 'admin/community/hats/' . $hat->name . '/delete'));
    
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
 * Page callback for editing a hat.
 *
 * @param $hat
 *  A hat object.
 */
function crm_party_hat_edit($hat) {
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
  
  dsm($hat);
  
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
    '#default_value' => $hat->required,
    '#description' => 'Whether (individual) parties require this hat.',
    '#weight' => 5,
  );
  
  foreach (crm_party_get_data_set_info() as $type => $set) {
    // Get the rule.
    if (isset($hat->name)) {
      $default = crm_party_hat_get_data_set_rule($hat->name, $type);
    }
    else {
      $default = CRM_PARTY_HAT_NOT_HAVE_SET;
    }
    
    $form['data_set_rules'][$type] = array(
      '#type' => 'radios',
      '#title' => t($set['label']),
      '#default_value' => $default,
      '#options' => array(
        CRM_PARTY_HAT_NOT_HAVE_SET => t("Doesn't have"),
        CRM_PARTY_HAT_MUST_HAVE_SET => t("Must have"),
        CRM_PARTY_HAT_CAN_HAVE_MULTIPLE => t("Must have and can have multiple"),
      ),
    );      
  }
  
  $form['data_set_rules'] = array(
    '#type' => 'fieldset',
    '#title' => t('Data Set Rules'),
    '#description' => 'Set rules for which data set these hats give',
    '#weight' => 5,
    '#tree' => true,
  );
  
  foreach (crm_party_get_data_set_info() as $type => $set) {
    //get the rule
    if (isset($hat->name)) {
      $default = crm_party_hat_get_data_set_rule($hat->name, $type);
    }
    else {
      $default = CRM_PARTY_HAT_NOT_HAVE_SET;
    }
    
    $form['data_set_rules'][$type] = array(
      '#type' => 'radios',
      '#title' => t($set['label']),
      '#default_value' => $default,
      '#options' => array(
        CRM_PARTY_HAT_NOT_HAVE_SET => t("Doesn't have"),
        CRM_PARTY_HAT_MUST_HAVE_SET => t("Must have"),
        CRM_PARTY_HAT_CAN_HAVE_MULTIPLE => t("Can have multiple"),
      ),
    );      
  }    
  
  $form['party_piece_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('party piece settings'),
    '#weight' => 10,
    '#tree' => true,
  );
  
  $form['party_piece_settings']['placeholder'] = array(
    '#markup' => '<i>This is a placeholder, in future hats will be able to define party pieces that can have views & data sets inside them.</i>',
  );
  
  $form['associated_roles'] = array(
    '#type' => 'fieldset',
    '#title' => t('Associated Roles'),
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
 * Form validation for crm_party_hat_edit_form
 */
function crm_party_hat_edit_form_validate($form, &$form_state) {
  $submission = (object) $form_state['values'];
  field_attach_form_validate('crm_party_hat', $submission, $form, $form_state);
}

/**
 * Form submission for crm_party_hat_edit_form
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
  
  foreach ($form_state['values']['data_set_rules'] as $type => $value) {
    crm_party_hat_set_data_set_rule($hat->name, $type, $value);
  }
  
  $form_state['redirect'] = 'admin/community/hats';  
}

/**
 * Check whether a machine name already exists
 */
function crm_party_hat_hat_machine_name_exists($value) {
  $exists = db_query_range('SELECT 1 FROM {crm_party_hats} WHERE `name` = :name', 0, 1, array(':name' => $value))->fetchField();
  return $exists;
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

/**
 * Settings form for ordering the party pieces.
 */
function crm_party_settings_pieces_order_form($form, &$form_state) {
  // Get the pieces. These come in with the stored weight setting already.
  $pieces = crm_party_get_party_piece_info();

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
function theme_crm_party_settings_pieces_order_form($variables) {
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
function crm_party_settings_pieces_order_form_submit($form, &$form_state) {
  $settings = array();
  foreach (array_keys($form_state['values']['pieces']) as $path) {
    $settings[$path] = $form_state['values']['pieces'][$path]['weight'];
  }

  variable_set('crm_party_name_pieces_weights', $settings);
  menu_rebuild();
}
