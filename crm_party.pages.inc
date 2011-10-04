<?php
/**
 * @file party.pages.inc
 */

/**
 * Page callback; displays a party.
 */
function crm_party_page_view($party, $view_mode = 'full') {
  $party->content = array();
  $build = array();

  $build = module_invoke_all('crm_party_page_view_alter', $party);
  // @todo: break this up with a hook_crm_party_section_info
  // where the following modules define a tab:
  // - party defines the first base tab
  // - user adds a tab if there is a linked user
  // - crm_profile adds tabs per profile type
  // - ... further modules here.  
  // These also need working into hook_menu as 2nd level tabs.

  $controller = entity_get_controller('crm_party');
  $build['party'] = $controller->view(array($party->pid => $party));
  return $build;
}

/**
 * Page title callback for party view page.
 */
function crm_party_page_title($party) {
  return $party->label;
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
  if (isset($party->pid)) {
    $form['pid'] = array(
      '#type' => 'value',
      '#value' => $party->pid,
    );

    drupal_set_title("Edit ".$party->label);
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
