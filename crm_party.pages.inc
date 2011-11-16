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

  $hats = crm_party_get_hats($party);
  $markup = '<ul>';
  foreach($hats as $hat) {
    $markup .= '<li>'.$hat->label.'</li>';
  }
  $markup .= '</ul>';
  
  $build['hats'] = array(
    '#markup' => $markup,
  );
  
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
  
  //Get all available hats
  $hats = crm_party_get_all_hats();
  $options = array();
  foreach($hats as $hat) {
    $options[$hat->name] = $hat->label;
  }
  
  //get hats associated with this party
  // @todo stop from unsetting required hats
  $party_hats = crm_party_get_hats($party);
  $defaults = array();
  foreach ($party_hats as $ph) {
    $defaults[] = $ph->name;
  }   
  
  $form['hats'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Hats'),
    '#options' => $options,
    '#default_value' => $defaults,
  );
  
  // Get Party Hat must have data sets
  $data_sets = array();
  foreach ($party_hats as $hat) {
    $temp_sets = crm_party_hat_get_data_sets($hat);
    foreach ($temp_sets as $set) {
      $data_sets[] = $set;
    }
  }

  $form_state['#party'] = $party;
  $form_state['#attached_entities'] = array();
  
  foreach ($data_sets as $data_set) {
    $attached_entities = array();
    if (isset($party->pid)) {
      $attached_entities = crm_party_get_attached_entities($party, $data_set);
    }
    
    if (count($attached_entities) < 1) {
      $attached_entity = crm_party_get_crm_controller($data_set);
      $attached_entity->setParty($party);
      $attached_entities[$attached_entity->hash()] = $attached_entity;
    }    
    
    $form_state['#attached_entities'] = array_merge($form_state['#attached_entities'], $attached_entities);
  }
  
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
  
  //This turns to true if a hat has been added that needs more information
  $hat_redirect = FALSE;
  //Do hats
  foreach ($form_state['values']['hats'] as $hat => $checked) {
    if ($checked === 0) {
      crm_party_hat_unassign($party, $hat);
    }
    else {
      crm_party_hat_assign($party, $hat);
      //check that all the data_sets are present
      //If they are not, redirect back to the form for them
      //to be added.
    }
  }
  
  $form_state['crm_party'] = $party;
  $form_state['#party'] = $party;
  $form_state['redirect'] = 'party/' . $party->pid;
}

/**
 * This checks if all expected data sets are present and, if not, sends the user
 * back to the form with a nice message.
 */
function crm_party_edit_form_hat_check($form, &$form_state) {
  foreach ($form_state['values']['hats'] as $hat => $checked) {
    $data_sets = array();
    foreach ($party_hats as $hat) {
      $temp_sets = crm_party_hat_get_data_sets($hat);
      foreach ($temp_sets as $set) {
        $data_sets[] = $set;
      }
    }
    
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
 * @param $set
 *   'The set type'
 * @param $pid
 *    The id of the party
 * @param $eid
 *    The id of the entity
 */
function crm_party_data_set_edit($set, $pid, $eid = NULL) {
  if ($eid === NULL) {
    //return "@todo add add form";
  }
  
  if (is_object($pid)) {
    $pid = $pid->pid;  
  }
  
  $attached_entity = crm_party_get_crm_controller($set);
  $attached_entity->setParty($pid);
  
  if ($eid !== NULL) {
    $attached_entity->setAttachedEntity($eid);
  }
  
  return drupal_get_form('crm_party_data_set_edit_form', $attached_entity);
}

/**
 * The data set edit form.
 */
function crm_party_data_set_edit_form($form, &$form_state, $dataSet) {
  $form['#dataSet'] = $dataSet;
  
  $form['data-set-type'] = array(
    '#type' => 'hidden',
    '#value' => $dataSet->type,
  );

  $form['data-set-pid'] = array(
    '#type' => 'hidden',
    '#value' => $dataSet->pid,
  );
  
  $form['data-set-eid'] = array(
    '#type' => 'hidden',
    '#value' => $dataSet->eid,
  );
  
  $dataSet->form($form, $form_state);
  
  if (!$dataSet->definition['singleton']) {
    $form['data-set-main'] = array(
      '#type' => 'checkbox',
      '#default_value' => $dataSet->isMain(),
      '#title' => "This is the main " . EntityDefaultCRMController::getTypeLabel($dataSet->type),
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
function crm_party_data_set_remove($set, $pid, $eid) {
  if(is_object($pid)) {
    $pid = $pid->pid;  
  }
  
  $info = array(
    'party' => $pid,
    'entity' => $eid,
  );
  
  $dataSet = EntityDefaultCRMController::getInstance($set,$info);
  drupal_set_title(t('Are you sure you want to remove ' . $dataSet->getLabel() . ' from ' . $dataSet->party->label . '?'));
  return drupal_get_form('crm_party_data_set_remove_confirm', $dataSet);
}

/**
 * Data set remove form
 */
function crm_party_data_set_remove_confirm($form, &$form_state, $dataSet) {
  $form['data-set-type'] = array(
    '#type' => 'hidden',
    '#value' => $dataSet->type,
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
    $build[$crm_controller->type . '_' . $crm_controller->id] = array(
        // @todo: this is sort of an abuse of what fieldsets are meant for... :/
        '#type' => 'fieldset',
        // @todo: stop this depending on $entity being a drupal entity
        // really? anything that's not an Entity on D7 isn't worth bothering with!
        '#title' => $crm_controller->getLabel(),
      );
    
    $build[$crm_controller->type . '_' . $crm_controller->id]['view'] = $crm_controller->display($crm_controller::VIEW_MODE);
 
    // @todo: change these to contextual links.
    $build[$crm_controller->type . '_' . $crm_controller->id]['actions'] = array(
      '#theme' => 'links',
      '#links' => $crm_controller->actions(),
      '#attributes' => array(
        'class' => array('crm-set-action-links'),
      ),
    );    
  }
  
  $set_definition = EntityDefaultCRMController::getSetDefinition($set);
  if (!isset($set_definition['max cardinality']) || count($attached_entity_controllers) < $set_definition['max cardinality']) {
    $build[$set . '_links'] = array(
      '#theme' => 'links',
      '#links' => array(
        'add' => array(
          'title' => "Add " . EntityDefaultCRMController::getTypeLabel($set),
          'href'  => 'party/' . $party->pid . '/add/' . $set,
        ),
      ),
      '#attributes' => array('class' => array('crm-profile-links')),
      '#weight' => 1,
    );
  }
  
  return $build;
}
