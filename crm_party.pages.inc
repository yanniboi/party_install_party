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
    // @todo: use theme_item_list().
    return "<ul class=\"admin-list\">
              <li class=\"leaf\">".l(t('Add Individual'), 'party/add/individual')."<div class=\"description\">Add an individual party</div></li>
              <li class=\"leaf\">".l(t('Add Organisation'), 'party/add/organisation')."<div class=\"description\">Add an organisation party</div></li>
            </ul>";
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
  
  $form['#submit'][] = 'crm_party_edit_form_hat_check';
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
 * This checks if all expected data sets are present and, if not, sends the user
 * back to the form with a nice message.
 *
 * @todo: Move to crm_party_hats
 */
function crm_party_edit_form_hat_check($form, &$form_state) {
  foreach (crm_party_hat_get_hats($form_state['#party']) as $hid => $hat) {
    $data_sets = crm_party_hat_get_data_sets($hat);
        
    foreach($data_sets as $data_set_name) {
      $attached_entities = crm_party_get_attached_entities($form_state['#party'], $data_set_name);
      if (count($attached_entities) < 1) {
        $form_state['redirect'] = 'party/' . $form_state['#party']->pid . '/party/edit';
        drupal_set_message("$party->label is expected to have a $data_set_name which wasn't present. Please enter the data now");
      }
    }
  }
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
 * Show the edit form for an individual data set.
 *
 * @todo: there seems to be no reason why this and crm_party_data_set_edit_form()
 * can't be combined.
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
function crm_party_data_set_edit($data_set, $party, $eid = NULL) {
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
  }
  
  return drupal_get_form('crm_party_data_set_edit_form', $attached_entity);
}

/**
 * The data set edit form.
 *
 * @param $attached_entity
 *  An attached entity controller.
 *  @todo: allow this to be NULL for adding new entities.
 */
function crm_party_data_set_edit_form($form, &$form_state, $attached_entity_controller) {
  $form['#dataSet'] = $attached_entity_controller;
  
  $form['data-set-type'] = array(
    '#type' => 'hidden',
    '#value' => $attached_entity_controller->data_set_name,
  );

  $form['data-set-pid'] = array(
    '#type' => 'hidden',
    '#value' => $attached_entity_controller->pid,
  );
  
  $form['data-set-eid'] = array(
    '#type' => 'hidden',
    '#value' => $attached_entity_controller->eid,
  );
  
  $attached_entity_controller->form($form, $form_state);
  
  if (!$attached_entity_controller->definition['singleton']) {
    $form['data-set-main'] = array(
      '#type' => 'checkbox',
      '#default_value' => $attached_entity_controller->isMain(),
      '#title' => "This is the main " . $attached_entity_controller->getTypeLabel(),
    );
  }

  $form['#submit'][] = 'crm_party_data_set_edit_form_submit';
  
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
function crm_party_data_set_edit_form_validate($form, &$form_state) {
  
}

/**
 * Submit handler for the data set edit form.
 */
function crm_party_data_set_edit_form_submit($form, &$form_state) {
  $type = $form_state['values']['data-set-type'];
  $info = array(
    'party' => $form_state['values']['data-set-pid'],
    'entity' => $form_state['values']['data-set-eid'],
  );
  
  if (!is_numeric($info['entity'])) {
    $sets = crm_party_get_data_set_info();
    $class = $sets[$type]['class'];
    
    $info['entity'] = $form_state['entity']->{$class::ENTITY_ID_KEY};
  }
  
  // @todo: this is available in the form.
  $dataSet = EntityDefaultCRMController::getInstance($type, $info);
  
  $options = array(
    'main' => $form_state['values']['data-set-main'],
  );
  $dataSet->attach($dataSet->party, $options);
  
  // Redirect to the set piece.
  $form_state['redirect'] = 'party/' . $dataSet->pid . '/' . $dataSet->getPathArg();
}

/**
 * Remove a data set from a party
 */
function crm_party_data_set_remove($data_set, $pid, $eid) {
  if(is_object($pid)) {
    $pid = $pid->pid;  
  }
  
  $info = array(
    'party' => $pid,
    'entity' => $eid,
  );
  
  $attached_entity_controller = EntityDefaultCRMController::getInstance($data_set['set_name'], $info);
  // @todo: fix misuse of t().
  drupal_set_title(t('Are you sure you want to remove ' . $attached_entity_controller->getLabel() . ' from ' . $attached_entity_controller->party->label . '?'));
  return drupal_get_form('crm_party_data_set_remove_confirm', $attached_entity_controller);
}

/**
 * Data set remove form
 */
function crm_party_data_set_remove_confirm($form, &$form_state, $dataSet) {
  $form['data-set-type'] = array(
    '#type' => 'hidden',
    '#value' => $dataSet->data_set_name,
  );   
  
  $form['data-set-id'] = array(
    '#type' => 'hidden',
    '#value' => $dataSet->id,
  );
  
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
function crm_party_data_set_remove_confirm_submit($form, &$form_state) {
  $type = $form_state['values']['data-set-type'];
  $peid = $form_state['values']['data-set-id'];
  
  $dataSet = EntityDefaultCRMController::getInstance($type, $peid, EntityDefaultCRMController::FROM_PEID);
  $pid = $dataSet->pid;
  $dataSet->detach();
  
  $form_state['redirect'] = 'party/' . $pid;
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
        // @todo: stop this depending on $entity being a drupal entity
        // really? anything that's not an Entity on D7 isn't worth bothering with!
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
