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
  'title' => t("Pid example"),
  'description' => t('Form a label from the party id.'),
  'label callback' => 'party_pid_label',
  'options form callback' => 'party_pid_options_form',
);

/**
 * Generate the label for a party.
 */
function party_pid_label($party) {
  // @todo. This is broken as inserted parties have no pid at this point!
  return "Party $party->pid";
}
