<?php


/**
 * The main API function for creating parties
 *
 * See party_devel_generate_parties_form() for the supported keys in
 * $form_state['values']. Other modules may participate by form_alter() on that
 * form and then handling their data during presave or in own submit handler
 * for the form.
 *
 * @param string $form_state
 * @return void
 */
function party_devel_generate_parties($form_state) {
  if (!empty($form_state['values']['kill_parties'])) {
    party_devel_generate_parties_kill($form_state['values']);
  }

  if (count($form_state['values']['party_types'])) {
    party_devel_generate_parties_pre_party($form_state['values']);
    $start = time();
    for ($i = 1; $i <= $form_state['values']['num_parties']; $i++) {
      // Get list of users we will use.
      party_devel_generate_party_add_party($form_state['values']);
      if (function_exists('drush_log') && $i % drush_get_option('feedback', 1000) == 0) {
        $now = time();
        drush_log(dt('Completed !feedback partiess (!rate party/min)', array('!feedback' => drush_get_option('feedback', 1000), '!rate' => (drush_get_option('feedback', 1000)*60)/($now-$start))), 'ok');
        $start = $now;
      }
    }
  }

  devel_generate_set_message(format_plural($form_state['values']['num_parties'], '1 party created.', 'Finished creating @count parties'));
}

/**
 * Handle the party_devel_generate_parties_form request to kill all of the
 * existing products.
 *
 * This is used by both the batch and non-batch branches of the code.
 *
 * @param $values
 *   array of options obtained from commerce_devel_generate_products_form.
 */
function party_devel_generate_parties_kill($values) {
  $results = db_select('party', 'p')
              ->fields('p', array('pid'))
              ->condition('type', $values['party_types'], 'IN')
              ->execute();
  foreach ($results as $result) {
    $party_ids[] = $result->pid;
  }

  if (!empty($party_ids)) {
    party_delete_multiple($party_ids);
    drupal_set_message(t('Deleted %count parties.', array('%count' => count($party_ids))));
  }
}

/**
 * Create one party. Used by both batch and non-batch code branches.
 *
 * @param $results
 *   array of options
 */
function party_devel_generate_party_add_party(&$results) {
  $type = array_rand($results['party_types']);

  $party = array(
    'type' => $type,
  );

  $party = party_create($party);

  // Insert new data:
  $party->label = devel_create_greeking(mt_rand(2, $results['label_length']), TRUE);

  // Save default parameters back into the $product object.
  $party->merged = 0;

  $party->language = LANGUAGE_NONE;

  // Save party fields
  module_load_include('inc', 'devel_generate', 'devel_generate.fields');
  devel_generate_fields($party, 'party', $party->type);

  // save the party
  $party->save();

  $users = $results['users'];

  // If party_user is enabled sometimes attach a user
  if (module_exists('party_user') && rand(0, 2) > 1) {
    $uid = $users[array_rand($users)];
    party_attach_entity($party, $uid, 'user');
  }

  // Attach a selection of data sets
  $data_sets = party_get_party_data_sets($party);

  foreach($data_sets as $id => $name) {
    if ($name == 'user') {
      continue;
    }

    $data_set_controller = party_get_crm_controller($name);
    $data_set_controller->createAttachedEntity();

    // Devel generate requires that a language be set.
    if (!isset($data_set_controller->entity->language)) {
      $data_set_controller->entity->language = LANGUAGE_NONE;
    }

    module_load_include('inc', 'devel_generate', 'devel_generate.fields');
    devel_generate_fields($data_set_controller->entity, $data_set_controller->getEntityType(), $data_set_controller->getEntityBundle());
    $data_set_controller->saveAttachedEntity();
    $data_set_controller->attach($party);
  }
}