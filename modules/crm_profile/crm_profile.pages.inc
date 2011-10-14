<?php 
/**
 * @file
 * Define pages needed for the crm_profile module
 */

/**
 * Page callback for our data set-based party pieces.
 */
function crm_profile_party_view_profile_set($party, $profile_type) {
  $build = array();

  // Load the profiles in this data set.
  $profiles = crm_party_data_set_load($party, "profile2_" . $profile_type);
  
  foreach ($profiles as $profile) {
    if (profile2_access('view', $profile)) {
      $profile->this_party = $party;

      $build['profile_' . $profile->pid] = array(
        '#type' => 'fieldset',
        '#title' => $profile->label,
      );
      $build['profile_' . $profile->pid]['view'] = $profile->view('party');
    }
  }

  $entity_info_profile = entity_get_info('profile2');
  $label = $entity_info_profile['bundles'][$profile_type]['label'];
  $build['profile-links'] = array(
    '#theme' => 'links',
    '#links' => array(
      'add' => array(
        'title' => "Add " . $label,
        'href'  => 'party/' . $party->pid . '/add/' . $profile_type,
      ),
    ),
    '#attributes' => array('class' => array('crm-profile-links')),
    '#weight' => 1,
  );

  return $build;
}

/**
 * Make the profile edity page
 */
function profile_page_edit($profile) {
  //TODO: do some magic and work out the page title. 
  //This works for now but will break once parties share the same profile
  $party = reset(_crm_profile_load_parties($profile));
  drupal_set_title(t("<em>Edit @party</em> @profile", array('@party' => $party->label, '@profile' => $profile->label)), PASS_THROUGH);
  return drupal_get_form("profile_edit_form", $profile, $party);
}

/**
 * Make the profile add page
 */
function profile_page_add($party,$profile_type) {
  //TODO: do some magic and work out the page title. 
  //This works for now but will break once parties share the same profile
  print_r($profile_type);
  $profile = profile_create(array('type' => $profile_type->type));
  drupal_set_title(t("Add @profile to @party", array('@party' => $party->label, '@profile' => $profile_type->label)), PASS_THROUGH);
  return drupal_get_form("profile_edit_form", $profile, $party);
}

/**
 * Make a form to edit an individual profile
 *
 * The Party is the Party we will Redirect Back to.
 */ 
function profile_edit_form($form, &$form_state, $profile, $party) {
  // @todo: don't stash things in the form state; put them either
  // in the form as hash properties or values.
  $form_state['profiles'][$profile->type] = $profile;
  $form_state['party'] = $party;
  profile2_attach_form($form, $form_state);

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 99,
  );
  
  dsm($profile);
  
  if (!isset($profile->pid)) {
    $form['#submit'][] = "profile_add_form_submit";
  }
  
  $form['#submit'][] = "profile_edit_form_submit";
  return $form;
}

/**
 * Submit handler for profile edit for.
 *
 * Currently this just sets the redirect.
 */
function profile_edit_form_submit($form, &$form_state) {
  $form_state['redirect'] = 'party/' . $form_state['party']->pid;
}

/**
 * Add a new profile to the party.
 */
function profile_add_form_submit($form, &$form_state) {
  // Relate the party to the new profile
  $party = $form_state['party'];
  
  foreach($form_state['profiles'] as $type => $profile) {
    crm_party_relate($party, $profile, "profile2_" . $type);
  }
  
  //TODO: break the relation to the null profile for this type.
}
