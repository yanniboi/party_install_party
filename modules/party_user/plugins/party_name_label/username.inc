<?php

/**
 * @file
 *
 * Sample plugin to output just the party id as a label.
 * This is just a proof of concept / example.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 *
 * @todo: document our plugin settings somewhere!
 *  - 'title': The admin name of the plugin.
 *  - 'description': Description for the admin UI.
 *  - 'label callback': The callback to use for generating a label.
 *    The signature is $callback($party).
 *  - 'options form callback': The callback for the settings form.
 *    Eg, for a FieldAPI label plugin this would let you choose which field(s)
 *    to take the label text from.
 */
$plugin = array(
  'title' => t("Username"),
  'description' => t('Form a label from the Name of the User Connected to a party'),
  'label callback' => 'party_user_username_label',
);

/**
 * Generate the label for a party.
 */
function party_user_username_label($party) {
  $attached_entity = reset($party->data_sets['user']);
  $user = $attached_entity->entity;
  
  return $user->name;
}
