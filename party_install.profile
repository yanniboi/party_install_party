<?php
/**
 * @file
 * Enables modules and site configuration for a party site installation.
 */

/**
 * Implements hook_form_FORM_ID_alter() for install_configure_form().
 *
 * Allows the profile to alter the site configuration form.
 */
function party_install_form_install_configure_form_alter(&$form, $form_state) {
  // Pre-populate the site name with the server name.
  // $form['site_information']['site_name']['#default_value'] = $_SERVER['SERVER_NAME'];

  // Hide some messages from various modules that are just too chatty!
  drupal_get_messages('status');
  drupal_get_messages('warning');

  // Set reasonable defaults for site configuration form
  $form['site_information']['site_name']['#default_value'] = 'Party';
  $form['admin_account']['account']['name']['#default_value'] = 'admin';
  // What is the default value for London?
   $form['server_settings']['site_default_country']['#default_value'] = 'United Kingdom';
   $form['server_settings']['date_default_timezone']['#default_value'] = 'Europe/London'; // The Party happens in the North West though!! 

  // Enable the admin theme.
  db_update('system')
    ->fields(array('status' => 1))
    ->condition('type', 'theme')
    ->condition('name', 'seven')
    ->execute();
  variable_set('admin_theme', 'seven');
  variable_set('node_admin_theme', '1');

  // Add text formats.
  $filtered_html_format = array(
    'format' => 'filtered_html',
    'name' => 'Filtered HTML',
    'weight' => 0,
    'filters' => array(
      // URL filter.
      'filter_url' => array(
        'weight' => 0,
        'status' => 1,
      ),
      // HTML filter.
      'filter_html' => array(
        'weight' => 1,
        'status' => 1,
      ),
      // Line break filter.
      'filter_autop' => array(
        'weight' => 2,
        'status' => 1,
      ),
      // HTML corrector filter.
      'filter_htmlcorrector' => array(
        'weight' => 10,
        'status' => 1,
      ),
    ),
  );
  $filtered_html_format = (object) $filtered_html_format;
  filter_format_save($filtered_html_format);

  // Enable default permissions for system roles.
  $filtered_html_permission = filter_permission_name($filtered_html_format);
  user_role_grant_permissions(DRUPAL_ANONYMOUS_RID, array('access content', 'access comments', $filtered_html_permission));
  user_role_grant_permissions(DRUPAL_AUTHENTICATED_RID, array('access content', 'access comments', 'post comments', 'skip comment approval', $filtered_html_permission));

  // Create a default role for site administrators, with all available permissions assigned.
  $admin_role = new stdClass();
  $admin_role->name = 'administrator';
  $admin_role->weight = 2;
  user_role_save($admin_role);
  user_role_grant_permissions($admin_role->rid, array_keys(module_invoke_all('permission')));
  // Set this as the administrator role.
  variable_set('user_admin_role', $admin_role->rid);

  // Assign user 1 the "administrator" role.
  db_insert('users_roles')
    ->fields(array('uid' => 1, 'rid' => $admin_role->rid))
    ->execute();


  // Create a list of Profile2s to be used as Data Sets
  $type = entity_create('profile2_type', array(
    'type' => 'individual',
    'label' => t('Individual'),
    'weight' => -3,
    'data' => array('registration' => TRUE, 'use_page' => TRUE),
  ));
  $type->save();
  $type = entity_create('profile2_type', array(
    'type' => 'staff',
    'label' => t('Staff'),
    'weight' => -2,
    'data' => array('registration' => TRUE, 'use_page' => TRUE),
  ));
  $type->save();
  $type = entity_create('profile2_type', array(
    'type' => 'student',
    'label' => t('Student'),
    'weight' => -1,
    'data' => array('registration' => TRUE, 'use_page' => TRUE),
  ));
  $type->save();
  $type = entity_create('profile2_type', array(
    'type' => 'organisation',
    'label' => t('Organisation'),
    'weight' => 0,
    'data' => array('registration' => TRUE, 'use_page' => TRUE),
  ));
  $type->save();
  $type = entity_create('profile2_type', array(
    'type' => 'main',
    'label' => t('Main'),
    'weight' => 0,
    'data' => array('registration' => TRUE, 'use_page' => TRUE),
  ));
  $type->save();

  // Create Name field for Main profile2
  $field = array(
    'field_name' => 'field_individual_name',
    'type' => 'text',
  );
  $field = field_create_field($field);

  // Attach newly created field to the profile2
  $instance = array(
    'field_name' => 'field_individual_name',
    'entity_type' => 'profile2',
    'bundle' => 'main',
    'label' => 'Name',
    'description' => t('Name of the individual'),
    'settings' => array(
      'text_processing' => 0,
    ),
    'widget' => array('type' => 'text_textfield'),
    'weight' => 11,
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'type' => 'text_plain',
      ),
      'party' => array(
        'label' => 'hidden',
        'type' => 'text_plain',
      ),
    ),
  );
  $instance = field_create_instance($instance);

  // Create Address field for Main profile2
  $field = array(
    'field_name' => 'field_individual_address',
    'type' => 'text_long',
  );
  $field = field_create_field($field);

  // Attach newly created fields to the Profile2
  $instance = array(
    'field_name' => 'field_individual_address',
    'entity_type' => 'profile2',
    'bundle' => 'main',
    'label' => 'Address',
    'description' => t('Address of the individual'),
    'settings' => array(
      'text_processing' => 0,
    ),
    'widget' => array('type' => 'text_textarea'),
    'weight' => 11,
    'display' => array(
      'default' => array(
        'label' => 'above',
        'type' => 'text_plain',
      ),
      'party' => array(
        'label' => 'above',
        'type' => 'text_plain',
      ),
    ),
  );
  $instance = field_create_instance($instance);

  // Create Email field for Main profile2
  $field = array(
    'field_name' => 'field_individual_email',
    'type' => 'email',
  );
  $field = field_create_field($field);

  // Attach newly created fields to the Profile2
  $instance = array(
    'field_name' => 'field_individual_email',
    'entity_type' => 'profile2',
    'bundle' => 'main',
    'label' => 'Email',
    'description' => t('Email of the individual'),
    'widget' => array('type' => 'email_textfield'),
    'weight' => 11,
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'type' => 'email_plain',
      ),
      'party' => array(
        'label' => 'hidden',
        'type' => 'email_plain',
      ),
    ),
  );
  $instance = field_create_instance($instance);
}

