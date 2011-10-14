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
              <li class=\"leaf\">".l(t('Add Organisation'), 'party/add/organisation')."Add Organisation</a><div class=\"description\">Add an organisation party</div></li>
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

  field_attach_form('crm_party', $party, $form, $form_state);

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 99,
  );

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
        $output = $view->execute_display($view_display_id);
        // @todo:
        //views_add_block_contextual_links($output, $view, $display_id);
        $view->destroy();
        return $output;
      }
    }
    $view->destroy();
  }
}

/**
 * Show the edit form for an individual data set
 *
 * @param $set
 *   'The set type'
 * @param $pid
 *    The id of the party
 * @param $eid
 *    The id of the entity
 */
function crm_party_data_set_edit($set, $pid, $eid) {
  if($eid === null) {
    return "@todo add add form";
  }
  
  $info = array(
    'party' => $pid,
    'entity' => $eid,
  );
  $dataSet = CRMPartyDataSet::getInstance($set,$info);
  return drupal_get_form('crm_party_data_set_edit_form', $dataSet);
}

/**
 * The data set edit form.
 */
function crm_party_data_set_edit_form($form, &$form_state, $dataSet) {
  $form['#dataSet'] = $dataSet;
  
  $form['dataSet'] = array(
    '#type' => 'hidden',
    '#value' => $dataSet->id,
  );
  
  $dataSet->form($form,$form_state);
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 99,
  );
  
  return $form;
}

/**
 * The data set edit validate.
 */
function crm_party_data_set_edit_form_validate($form, &$form_state) {
  
}

/**
 * The data set edit validate.
 */
function crm_party_data_set_edit_form_submit($form, &$form_state) {
  
}

/**
 * Remove a data set from a party
 */
function crm_party_data_set_remove($set, $pid, $eid) {
  $info = array(
    'party' => $pid,
    'entity' => $eid,
  );
  $dataSet = CRMPartyDataSet::getInstance($set,$info);
  drupal_set_title(t('Are you sure you want to remove ' . $dataSet->entity->label . ' from ' . $dataSet->party->label . '?'));
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
 * The data set edit validate.
 */
function crm_party_data_set_remove_confirm_submit($form, &$form_state) {
  $type = $form_state['values']['data-set-type'];
  $peid = $form_state['values']['data-set-id'];
  
  $dataSet = CRMPartyDataSet::getInstance($type, $peid, CRMPartyDataSet::FROM_PEID);
  $dataSet->detach();
  
  $form_state['#redirect'] = 'party/' . $dataSet->pid;
}