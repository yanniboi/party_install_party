<?php
/**
 * @file party.pages.inc
 */

/**
 * Page callback; displays a party.
 *
 * @todo: replace this completely now we have party pieces as 2nd level tabs.
 * this should be... what? basic info about the party like creation date,
 * label, etc?
 */
function crm_party_page_view($party, $view_mode = 'full') {
  $party->content = array();
  $build = array();

  $build = module_invoke_all('crm_party_page_view_alter', $party);

  // Demo output so we see this works.
  $build['demo'] = array(
    '#type' => 'markup',
    '#markup' => 'Party view page',
  );

  $controller = entity_get_controller('crm_party');
  $build['party'] = $controller->view(array($party->pid => $party));
  return $build;
}


/**
 * Page title callback for party edit page.
 */
function crm_party_edit_page_title($party) {
  return "Edit " . $party->label;
}

/**
 * Page callback for adding a party.
 */
function crm_party_add($type = '') {
  if ($type == '') {
    $build['add_list'] = array(
      '#theme' => 'item_list',
      '#type' => 'item_list',
      '#items' => array( 
        l(t('Add Individual'), 'party/add/individual')."<div class=\"description\">Add an individual party</div>",
        l(t('Add Organisation'), 'party/add/organisation')."<div class=\"description\">Add an organisation party</div>",
      ),
    );
    
    return $build;
  }

  $party = (object) array(
    'type' => $type,
    'label' => '',
  );

  return drupal_get_form('crm_party_edit_form', $party);
}

/**
 * Party edit form.
 */
function crm_party_edit_form($form, &$form_state, $party) {
  $form['#party'] = $party;

  if (isset($party->pid)) {
    $form['pid'] = array(
      '#type' => 'value',
      '#value' => $party->pid,
    );

    drupal_set_title("Edit " . $party->label);
  }

  $form['type'] = array(
    '#type' => 'hidden',
    '#value' => $party->type,
  );

  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Party Label'),
    '#required' => TRUE,
    '#default_value' => $party->label,
  );
  
  $form_state['#party'] = $party;
  $form_state['#attached_entities'] = array();
  
  //get hats associated with this party
  // @todo: Get into crm_party_hat module.
  $party_hats = crm_party_hat_get_hats($party);
  $defaults = array();
  if (!isset($party->pid)) {
    foreach ($party_hats as $ph) {
      $party->party_hat['und'][]['hat_name'] = $ph->name;
    }
  }    
  
  // Get Party Hat must have data sets
  $data_sets = array();
  foreach ($party_hats as $hat) {
    $temp_sets = crm_party_hat_get_data_sets($hat);
    foreach ($temp_sets as $set) {
      $data_sets[] = $set;
    }
  }

  foreach ($data_sets as $data_set) {
    $attached_entities = array();
    if (isset($party->pid)) {
      $attached_entities = crm_party_get_attached_entities($party, $data_set);
    }
    
    if (count($attached_entities) < 1) {
      $attached_entity = crm_party_get_crm_controller($data_set);
      $attached_entity->createAttachedEntity();
      $attached_entity->setParty($party);
      $attached_entities[$attached_entity->hash()] = $attached_entity;
    }    
    
    $form_state['#attached_entities'] = array_merge($form_state['#attached_entities'], $attached_entities);
  }
  // Everything between the last @todo and here needs to be in a hook so that other modules can change it.  
  
  $form['#validate'][] = 'crm_party_edit_form_validate';
  $form['#submit'][] = 'crm_party_edit_form_submit';
  
  crm_party_attached_entity_attach_form($form, $form_state);
  
  field_attach_form('crm_party', $party, $form, $form_state);

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 99,
  );
  
  if (module_exists('crm_party_hat')) {
    // @todo: Move this to before the attach function.
    $form['#submit'][] = 'crm_party_hat_party_form_hat_check';
  }
  return $form;
}

function crm_party_edit_form_validate($form, &$form_state) {
  $submission = (object) $form_state['values'];
  field_attach_form_validate('crm_party', $submission, $form, $form_state);
}

function crm_party_edit_form_submit($form, &$form_state) {
  $party = (object) $form_state['values'];
  field_attach_submit('crm_party', $party, $form, $form_state);
  $party = crm_party_save($party);
  
  $form_state['crm_party'] = $party;
  $form_state['#party'] = $party;
  $form_state['redirect'] = 'party/' . $party->pid;
}

/**
 * Show a View display plugin as a party piece.
 *
 * @param $party
 *  A party entity from the menu loader.
 * @param $view_name
 *  The machine name of a view.
 * @param $view_display_id
 *  The machine name of a view display.
 */
function crm_party_page_view_piece_views($party, $view_name, $view_display_id) {
  if ($view = views_get_view($view_name)) {
    if ($view->access($view_display_id)) {
      $view->set_display($view_display_id);
      if (isset($view->display_handler)) {
        $view->set_arguments(array($party->pid));
        $output = $view->execute_display($view_display_id);
        // @todo:
        //views_add_block_contextual_links($output, $view, $view_display_id);
        $view->destroy();
        return $output;
      }
    }
    $view->destroy();
  }
}

/**
 * The attached entity edit form.
 *
 * @param $data_set
 *   A loaded data set.
 *   We don't actually need this loaded, but we need a menu loader to convert
 *   the path-style string to the machine name, and a menu loader that doesn't
 *   load would be weird too.
 * @param $pid
 *    The id of the party
 * @param $eid
 *    The id of the entity
 */
function crm_party_attached_entity_edit_form($form, &$form_state, $data_set, $party, $eid = NULL) {
  
  // Get the attached entity object
  if (is_object($party)) {
    $pid = $party->pid;  
  }
  else {
    $pid = $party;
  }
  $attached_entity = crm_party_get_crm_controller($data_set['set_name']);
  $attached_entity->setParty($pid);
  
  if ($eid !== NULL) {
    $attached_entity->setAttachedEntity($eid);
    drupal_set_title("Edit Data Set: " . $attached_entity->entity->label);
  }
  else {
    drupal_set_title("Add {$attached_entity->definition['label']} Data Set to {$party->label}");
    $attached_entity->createAttachedEntity();
  }
  
  //Build form
  if (!isset($form_state['#attached_entities'][$attached_entity->hash()])) {
    $form_state['#attached_entities'][$attached_entity->hash()] = $attached_entity;
  }
  
  $form['#submit'][] = 'crm_party_attached_entity_edit_form_submit';
  $form_state['#party'] = $attached_entity->party;
  
  crm_party_attached_entity_attach_form($form, $form_state); 
  
  /*if (!$attached_entity_controller->definition['singleton']) {
    $form['data-set-main'] = array(
      '#type' => 'checkbox',
      '#default_value' => $attached_entity_controller->isMain(),
      '#title' => "This is the main " . $attached_entity_controller->getDataSetLabel(),
    );
  }*/

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 99,
  );
  
  return $form;
}

/**
 * Validation handler for the data set edit form.
 */
function crm_party_attached_entity_edit_form_validate($form, &$form_state) {
  return TRUE;
}

/**
 * Submit handler for the data set edit form.
 */
function crm_party_attached_entity_edit_form_submit($form, &$form_state) {
  
  /*   
  $options = array(
    'main' => $form_state['values']['data-set-main'],
  );
  $attached_entity_controller->attach($attached_entity_controller->party, $options); */
  
  $attached_entity_controller = reset($form_state['#attached_entities']);
  
  // Redirect to the set piece.
  $form_state['redirect'] = 'party/' . $attached_entity_controller->pid . '/' . $attached_entity_controller->getPathArg();
}

/**
 * Remove a data set from a party
 */
function crm_party_attached_entity_remove($data_set, $pid, $eid) {
  if(is_object($pid)) {
    $pid = $pid->pid;  
  }
  
  $attached_entity = crm_party_get_crm_controller($data_set['set_name']);
  $attached_entity->setAttachedEntity($eid);
  $attached_entity->setParty($pid);
  // @todo: fix misuse of t().
  drupal_set_title(t('Are you sure you want to remove ' . $attached_entity->getLabel() . ' from ' . $attached_entity->party->label . '?'));
  return drupal_get_form('crm_party_attached_entity_remove_confirm', $attached_entity);
}

/**
 * Data set remove form
 */
function crm_party_attached_entity_remove_confirm($form, &$form_state, $attached_entity) {
  $form_state['#attached_entity'] = $attached_entity;
  
  $form['description'] = array(
    '#markup' => '<p>Are you sure you want to remove this data set from this party?</p>',
  );
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Confirm'),
    '#weight' => 99,
  );
  
  return $form;
}

/**
 * The data set remove submit
 */
function crm_party_attached_entity_remove_confirm_submit($form, &$form_state) {
  $form_state['#attached_entity']->detach();  
  $form_state['redirect'] = 'party/' . $form_state['#attached_entity']->party->pid;
}

/**
 * Page callback to view a data set.
 */
function crm_party_view_data_set($party, $set) {
  $attached_entity_controllers = crm_party_get_attached_entities($party, $set);

  foreach ($attached_entity_controllers as $crm_controller) {
    $entity = $crm_controller->entity;
    $build[$crm_controller->data_set_name . '_' . $crm_controller->id] = array(
        // @todo: this is sort of an abuse of what fieldsets are meant for... :/
        '#type' => 'fieldset',
        '#title' => $crm_controller->getLabel(),
      );
    
    $build[$crm_controller->data_set_name . '_' . $crm_controller->id]['view'] = $crm_controller->display($crm_controller::VIEW_MODE);
  }
  
  $set_definition = crm_party_get_data_set_info($set);
  if (!isset($set_definition['max cardinality']) || count($attached_entity_controllers) < $set_definition['max cardinality']) {
    $build[$set . '_links'] = array(
      '#theme' => 'links',
      '#links' => array(
        'add' => array(
          'title' => "Add " . $set_definition['label'],
          'href'  => 'party/' . $party->pid . '/add/' . $set,
        ),
      ),
      '#attributes' => array('class' => array('crm-profile-links')),
      '#weight' => 1,
    );
  }
  
  return $build;
}
