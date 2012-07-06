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
function party_page_view($party, $view_mode = 'full') {
  global $user;

  $party->content = array();
  $build = array();

  $build = module_invoke_all('party_page_view_alter', $party);

  // Demo output so we see this works.
  $build['demo'] = array(
    '#type' => 'markup',
    '#markup' => 'Party view page',
  );

  $controller = entity_get_controller('party');
  $build['party'] = $controller->view(array($party->pid => $party));
  return $build;
}


/**
 * Page title callback for party edit page.
 */
function party_edit_page_title($party) {
  return "Edit " . $party->label;
}

/**
 * Page callback for adding a party.
 */
function party_add() {
  $party = entity_create('party', array(
    'label' => '',
  ));

  return drupal_get_form('party_form', $party);
}

/**
 * Party edit form.
 */
function party_form($form, &$form_state, $party, $op = 'edit') {
  $form['#party'] = $party;

  if (isset($party->pid)) {
    $form['pid'] = array(
      '#type' => 'value',
      '#value' => $party->pid,
    );

    drupal_set_title("Edit " . $party->label);
  }

  $form_state['#party'] = $party;
  $form_state['#party_unchanged'] = $party; // So that other modules can react to changes. Must be a better way
  $form_state['#data_set_controllers'] =
  $form_state['#attached_entities'] = array();

  $data_sets = party_get_party_data_sets($party);

  foreach ($data_sets as $data_set) {
    // Get our controller
    $data_set_controller = party_get_crm_controller($data_set, $party);
    $attached_entities = array();

    // See if we can load any entities
    if (isset($party->pid)) {
      $attached_entities = $data_set_controller->getEntities();

      // If not, let's create one
      if (count($attached_entities) == 0) {
        $attached_entities[] = $data_set_controller->getEntity(0, TRUE);
      }
    }
    else {
      // No party so this has to be a little different, we'll attach
      // after the party is saved.
      $attached_entities[] = $data_set_controller->createEntity();
    }

    foreach ($attached_entities as $delta => $entity) {
      $form_state['#attached_entities'][$data_set . ':' . $delta] = $entity;
    }
  }
  // Everything between the last @todo and here needs to be in a hook so that other modules can change it.

  $form['#validate'][] = 'party_edit_form_validate';
  $form['#submit'][] = 'party_edit_form_submit';

  party_attached_entity_attach_form($form, $form_state);

  field_attach_form('party', $party, $form, $form_state);

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 99,
  );

 $form['#submit'][] = 'party_edit_form_save';
  return $form;
}

/**
 * Validate the Party Edit Form
 *
 * @todo: Add validation for all the attached entities
 */
function party_edit_form_validate($form, &$form_state) {
  $pseudo_entity = (object) $form_state['values'];
  field_attach_form_validate('party', $pseudo_entity, $form, $form_state);
}

/**
 * Submit handler for the the Party edit form.
 *
 * The work of saving the party is split over 3 submit handlers for this form:
 *  - party_edit_form_submit() - builds the Party object and attaches field API
 *    field values.
 *  - party_attached_entity_attach_form_submit() - saves all the attached
 *    entities and adds them to the party->data_sets array.
 *  - party_edit_form_save() - attaches all the attached entities and saves the
 *    party.
 * This is in order to allow plugins to use all of the party data available when
 * generating the name label.
 */
function party_edit_form_submit($form, &$form_state) {
  $party = $form_state['#party'];
  field_attach_submit('party', $party, $form, $form_state);
  party_save($party);

  $party->data_sets = array();

  $form_state['#party'] = $party;
}

/**
 * Attach entities and save the party
 *
 * @see party_edit_form_submit
 */
function party_edit_form_save($form, &$form_state) {
  $party = $form_state['#party'];

  // Load our attached data sets
  $data_sets = party_get_party_data_sets($party);
  $data_set_info = party_get_data_set_info();

  foreach ($data_sets as $data_set) {
    if (!empty($party->data_sets[$data_set])) {
      $data_set_controller = party_get_crm_controller($data_set, $party);
      foreach ($party->data_sets[$data_set] as $entity) {
        $data_set_controller->attachEntity($entity);
      }
      $data_set_controller->save();
    }
  }

  // Now new entities have been attached (if necessary) set the party label.
  entity_get_controller('party')->setLabel($party);

  drupal_set_message(t("@party saved successfully.", array('@party' => $party->label)));

  $form_state['redirect'] = 'party/' . $party->pid;
}

/**
 * Party delete form
 */
function party_delete_form($form, &$form_state, $party) {
  $form_state['party'] = $party;

  $form = confirm_form($form,
    t('Are you sure you want to delete party %label (%id)?', array('%label' => $party->label, '%id' => $party->pid)),
    'admin/community',
    '<p>' . t('This action cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit the party delete form
 */
function party_delete_form_submit($form, &$form_state) {
  $form_state['party']->delete();
}

// =================================== Party piece display.

/**
 * Page callback to view a data set.
 *
 * Used for display of data sets that don't use Views or provide their own
 * page callback.
 *
 * @see party_party_party_pieces().
 *
 * @param $party
 *    A loaded party object.
 * @param $data_set_name
 *   A data set name.
 */
function party_view_data_set($party, $data_set_name) {
  // TODO: some of these calls are redundant!
  $data_set_controller = party_get_crm_controller('user', $party);
  $attached_entities = party_get_attached_entities($party, $data_set_name);

  $build = array();
  foreach ($attached_entities as $delta => $entity) {
    $build[$data_set_name . '_' . $delta] = array(
      // @todo: this is sort of an abuse of what fieldsets are meant for... :/
      '#type' => 'fieldset',
      '#title' => $data_set_controller->getLabel($delta),
    );

    $build[$data_set_name . '_' . $delta]['view'] = $data_set_controller->display($delta);
  }

  return $build;
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
function party_page_view_piece_views($party, $view_name, $view_display_id) {
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

// =================================== Forms for controller-based actions.

/**
 * Form builder for a generalized attached entity action.
 *
 * @todo: let this handle built-in actions: 'edit', 'delete', 'detach'.
 *
 * @param $party
 *   A loaded party object.
 * @param $data_set
 *   A loaded data set.
 *   We don't actually need this loaded, but we need a menu loader to convert
 *   the path-style string to the machine name, and a menu loader that doesn't
 *   load would be weird too.
 * @param $action
 *   The machine name of a data set action, as defined by the data set in
 *   hook_party_data_set_info().
 * @param $eid
 *   The id of the entity, if editing.
 */
function party_attached_entity_action_form($form, &$form_state, $party, $data_set, $action, $eid = NULL) {
  $action_controller = new $data_set['actions'][$action]['controller'];

  // Get the page title from the controller.
  $page_title = $action_controller->getPageTitle($party, $data_set, $eid);
  drupal_set_title($page_title);

  $form += $action_controller->action_form($form, $form_state, $party, $data_set, $eid);

  return $form;
}

/**
 * Validate handler for the general action form.
 */
function party_attached_entity_action_form_validate($form, &$form_state) {
  // TODO: write me: call the validate handler in the action controller.
}

/**
 * Submit handler for the general action form.
 */
function party_attached_entity_action_form_submit($form, &$form_state) {
  // Retrieve the original form parameters from the build info.
  // Minor WTF: because $eid is optional, it doesn't show in the array. Shove it
  // in there as NULL if missing, so controller handlers can rely on it.
  if (!isset($form_state['build_info']['args'][3])) {
    $form_state['build_info']['args'][3] = NULL;
  }
  list ($party, $data_set, $action, $eid) = $form_state['build_info']['args'];

  // Call the submit handler in the action controller.
  $action_controller = new $data_set['actions'][$action]['controller'];
  $action_controller->action_form_submit($form, $form_state, $party, $data_set, $eid);

  // Redirect to the set piece.
  $form_state['redirect'] = 'party/' . $party->pid . '/' . $data_set['path element'];
}

// =================================== Attached entity action forms.

/**
 * Form to edit or create an attached entity.
 *
 * @param $party
 *    A loaded party object.
 * @param $data_set
 *   A loaded data set.
 *   We don't actually need this loaded, but we need a menu loader to convert
 *   the path-style string to the machine name, and a menu loader that doesn't
 *   load would be weird too.
 * @param $eid
 *    The id of the entity, if editing.
 *
 * @todo: Reimplement actions and edit overlay stuff
 */
function party_attached_entity_edit_form($form, &$form_state, $party, $data_set, $eid = NULL) {
  $data_set_controller = party_get_crm_controller($data_set['set_name'], $party);
  // Load up the entities into the controller.
  $data_set_controller->loadEntities();

  if (isset($eid)) {
    $entity = reset(entity_load($data_set_controller->getDataInfo('entity type'), array($eid)));
    $form_title = t("Edit @data-set-label (@entity-label)", array(
      '@data-set-label' => $data_set['label'],
      '@entity-label' => entity_label($data_set['entity type'], $entity),
    ));
  }
  else {
    $entity = $data_set_controller->createEntity();
    $form_title = t("Add @data-set-label to @party-label", array(
      '@data-set-label' => $data_set_controller->getDataInfo('label'),
      '@party-label' => $party->label,
    ));
  }
  drupal_set_title($form_title);

  // Save the data set object in the form state
  $form_state['#partyData'] = $data_set_controller;

  // Build form
  if (!isset($form_state['#attached_entities'][$data_set_controller->getDataInfo('entity type') . ':' . $eid])) {
    $form_state['#attached_entities'][$data_set_controller->getDataInfo('entity type') . ':' . $eid] = $entity;
  }

  $form_state['#party'] = $party;

  // Get the attached entity form as a standalone, as we have just the one.
  party_attached_entity_attach_form($form, $form_state, TRUE);
  $form['#submit'][] = 'party_attached_entity_edit_form_submit';

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
function party_attached_entity_edit_form_validate($form, &$form_state) {
  return TRUE;
}

/**
 * Submit handler for the data set edit form.
 */
function party_attached_entity_edit_form_submit($form, &$form_state) {
  $entity = reset($form_state['#attached_entities']);
  $party = $form_state['#party'];
  $partyData = $form_state['#partyData'];

  if (!in_array($entity->{$partyData->getDataInfo('id key')}, $partyData->getEntityIds())) {
    $partyData->attachEntity($entity);
    $partyData->save();
  }

  // Redirect to the set piece.
  $form_state['redirect'] = 'party/' . $party->pid . '/' . $partyData->getDataInfo('path element');
}

/**
 * Form to confirm removing a data set from a party.
 */
function party_attached_entity_remove_confirm($form, &$form_state, $party, $data_set, $eid) {
  $data_set_controller = party_get_crm_controller($data_set['set_name']);
  $data_set_controller->setAttachedEntity($eid);
  $data_set_controller->setParty($party->pid);

  $form_state['#attached_entity'] = $data_set_controller;

  return confirm_form($form,
    t('Are you sure you want to remove %attached from %party?', array(
      '%attached' => $data_set_controller->getLabel(),
      '%party' =>  $data_set_controller->party->label,
    )),
    'party/' . $party->pid, // @todo: go back to the dataset page.
    t('Are you sure you want to remove this data set from this party?') . ' ' . t('This action cannot be undone.'),
    t('Remove'),
    t('Cancel')
  );
}

/**
 * The data set remove submit
 */
function party_attached_entity_remove_confirm_submit($form, &$form_state) {
  $form_state['#attached_entity']->detach();
  $form_state['redirect'] = 'party/' . $form_state['#attached_entity']->party->pid;
}
