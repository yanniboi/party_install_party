<?php 
/**
 * @file
 * Define pages needed for the crm_profile module
 */

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
  $form_state['profiles'][$profile->type] = $profile;
  $form_state['party'] = $party;
  profile2_attach_form($form, $form_state);

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 99,
  );
  
  dsm($profile);
  
  if(isset($profile->pid)) {
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
  $form_state['redirect'] = 'party/'.$form_state['party']->pid;
}

/**
 * Add a new profile to the party
 *
 */
function profile_add_form_submit($form, &$form_state) {
  //relate the two
  
  //break the relation to the null profile for this type.
}