<?php
/**
 * @file
 * Field Info for the CRM Party Hat Module
 */
 
/**
 * Implements hook_field_info()
 */
function crm_party_hat_field_info() {
  return array(
    'hat_reference' => array(
      'label' => t('Hat Reference'),
      'description' => t('Stores a Reference to a Hat'),
      'default_widget' => 'options_buttons',
      'default_formatter' => 'hat_reference_default',
      'module' => 'crm_party_hat',
      'settings' => array(),
      'instance_settings' => array(),
    )
  );
}

/**
 * Implements hook_field_settings_form()
 */
function crm_party_hat_field_settings_form($field, $instance, $has_data) {
  return array();
}

/**
 * Implements hook_field_validate()
 */
function crm_party_hat_field_validate($entity_type, $entity, $field, $instance, $langcode, $items, &$errors) {
}

/**
 * Implements hook_field_widget_error()
 */
function crm_party_hat_field_widget_error($element, $error, $form, &$form_state) {
}

/**
 * Implements hook_field_is_empty()
 */
function crm_party_hat_field_is_empty() {
  return FALSE;
}

/**
 * Implements hook_field_widget_info_alter()
 */
function crm_party_hat_widget_info_alter(&$info) {
  //Use options_select for the options
  $info['options_buttons']['field types'][] = 'hat_reference';
}

/**
 * Implements hook_options_list()
 */
function crm_party_hat_options_list($field, $instance = NULL) {
  $hats = crm_party_hat_get_all_hats();
  $options = array();
  
  foreach ($hats as $hat) {
    $options[$hat->name] = $hat->label;
  }
  
  return $options;  
}

/**
 * Implements hook_field_presave()
 */
function crm_party_hat_field_presave($entity_type, $entity, $field, $instance, $langcode, &$items) {

}

/**
 * Implements hook_field_formatter_info()
 *
 * Returns information about available field formatters.
 */
function crm_party_hat_field_formatter_info() {
  return array(
    'hat_reference_default' => array(
       'label' => t('Default'),
       'field types' => array('hat_reference'),
    ),
  );
}

/**
 * Implements hook_field_formatter_view()
 */
function crm_party_hat_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();
  
  switch ($display['type']) {
    case 'hat_reference_default':
      foreach($items as $delta => $item) {
        $hat = crm_party_hat_load($item['hat_name']);
        
        $element[$delta]['#markup'] = $hat->label;
      }
    break;
  }
  return $element;
}


