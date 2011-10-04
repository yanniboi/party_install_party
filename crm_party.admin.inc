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
  // To start of with we will just load the id.
  $header = array(
    'pid' => array('data' => t('Party Id'), 'field' => 'cp.pid'),
  );
  
  $query = db_select('crm_party','cp');
  $query->condition('cp.pid', 0, '<>');
  
  $count_query = clone $query;
  $count_query->addExpression('COUNT(cp.pid)');

  $query = $query->extend('PagerDefault')->extend('TableSort');
  $query
    ->fields('cp', array('pid'))
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
  
  // Builds the table list
  $destination = drupal_get_destination();
  
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