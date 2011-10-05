<?php
/**
 * @file crm_party.views.inc
 * Contains Views hooks.
 */

/**
 * Implements hook_views_plugins().
 */
function crm_party_views_plugins() {
  return array(
    'display' => array(
      'crm_party_piece' => array(
        'title' => t('Party component'),
        'help' => t("Display the view as a component for CRM parties."),
        'handler' => 'crm_party_plugin_display_party_piece',
        'theme' => 'views_view',
        'contextual links locations' => array(),
        //'theme' => 'views_attach_display_node_content_view',
        //'theme file' => 'views_attach.theme.inc',
        //'theme path' => drupal_get_path('module', 'views_attach'),
        //'uses hook nodeapi' => TRUE,
        'uses hook crm_party_party_pieces' => TRUE,
        'use ajax' => TRUE,
        'use pager' => TRUE,
        'use more' => FALSE,
        'accept attachments' => TRUE,
        'admin' => t('Party component'),
        //'help topic' => 'display-node-content',
      ),
    ),
  );
}
