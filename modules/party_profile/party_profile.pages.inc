<?php 
/**
 * @file
 * Define pages needed for the party_profile module
 */

/**
 * Page callback for our data set-based party pieces.
 */
function party_profile_party_view_profile_set($party, $profile_type) {
  $build = array();

  // Load the profiles in this data set.
  $sets = EntityDefaultPartyController::getInstances("profile2_" . $profile_type, $party);
  
  // Output each profile provided the user has view access.
  foreach ($sets as $set) {
    $profile = $set->entity;
    if (profile2_access('view', $profile)) {
      $build['profile_' . $profile->pid] = array(
        '#type' => 'fieldset',
        '#title' => $profile->label,
      );
      $build['profile_' . $profile->pid]['view'] = $profile->view('party');
      $build['profile_' . $profile->pid]['actions'] = array(
        '#theme' => 'links',
        '#links' => $set->actions(),
        '#attributes' => array(
          'class' => array('crm-set-action-links'),
        ),
      );
    }
  }

  // Add a link to add further profiles to this set.
  $entity_info_profile = entity_get_info('profile2');
  $label = $entity_info_profile['bundles'][$profile_type]['label'];
  $build['profile-links'] = array(
    '#theme' => 'links',
    '#links' => array(
      'add' => array(
        'title' => "Add " . $label,
        'href'  => 'party/' . $party->pid . '/add/profile2_' . $profile_type,
      ),
    ),
    '#attributes' => array('class' => array('crm-profile-links')),
    '#weight' => 1,
  );

  return $build;
}
