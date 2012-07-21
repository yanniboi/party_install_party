<?php
/**
 * @file Provide default views for the Party Business to Business Starterkit
 */

/**
 * Implement hook_views_default_views().
 */
function party_starterkit_b2b_views_default_views() {
  $views = array();

  // Contacts view.
  $view = new view;
  $view->name = 'contacts';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'party';
  $view->human_name = 'Contacts';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Contacts';
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['query_comment'] = FALSE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '10';
  $handler->display->display_options['style_plugin'] = 'table';
  /* Relationship: Party: Attached Profile */
  $handler->display->display_options['relationships']['eid_profile2']['id'] = 'eid_profile2';
  $handler->display->display_options['relationships']['eid_profile2']['table'] = 'party_attached_entity';
  $handler->display->display_options['relationships']['eid_profile2']['field'] = 'eid_profile2';
  $handler->display->display_options['relationships']['eid_profile2']['label'] = 'Attached Contact Profile';
  $handler->display->display_options['relationships']['eid_profile2']['required'] = 0;
  $handler->display->display_options['relationships']['eid_profile2']['data_set'] = 'profile2_party_contact';
  $handler->display->display_options['relationships']['eid_profile2']['main'] = 0;
  /* Relationship: Party: Relation: Employee of (party <-> party) */
  $handler->display->display_options['relationships']['relation_employee_party']['id'] = 'relation_employee_party';
  $handler->display->display_options['relationships']['relation_employee_party']['table'] = 'party';
  $handler->display->display_options['relationships']['relation_employee_party']['field'] = 'relation_employee_party';
  $handler->display->display_options['relationships']['relation_employee_party']['required'] = 0;
  $handler->display->display_options['relationships']['relation_employee_party']['r_index'] = '0';
  /* Field: Party: Party ID */
  $handler->display->display_options['fields']['pid']['id'] = 'pid';
  $handler->display->display_options['fields']['pid']['table'] = 'party';
  $handler->display->display_options['fields']['pid']['field'] = 'pid';
  $handler->display->display_options['fields']['pid']['label'] = '';
  $handler->display->display_options['fields']['pid']['exclude'] = TRUE;
  $handler->display->display_options['fields']['pid']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['pid']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['pid']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['pid']['alter']['external'] = 0;
  $handler->display->display_options['fields']['pid']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['pid']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['pid']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['pid']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['pid']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['pid']['alter']['more_link'] = 0;
  $handler->display->display_options['fields']['pid']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['pid']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['pid']['alter']['html'] = 0;
  $handler->display->display_options['fields']['pid']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['pid']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['pid']['hide_empty'] = 0;
  $handler->display->display_options['fields']['pid']['empty_zero'] = 0;
  $handler->display->display_options['fields']['pid']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['pid']['separator'] = '';
  $handler->display->display_options['fields']['pid']['format_plural'] = 0;
  /* Field: Party: Label */
  $handler->display->display_options['fields']['label']['id'] = 'label';
  $handler->display->display_options['fields']['label']['table'] = 'party';
  $handler->display->display_options['fields']['label']['field'] = 'label';
  $handler->display->display_options['fields']['label']['label'] = 'Contact Name';
  $handler->display->display_options['fields']['label']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['label']['alter']['make_link'] = 1;
  $handler->display->display_options['fields']['label']['alter']['path'] = 'party/[pid]';
  $handler->display->display_options['fields']['label']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['label']['alter']['external'] = 0;
  $handler->display->display_options['fields']['label']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['label']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['label']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['label']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['label']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['label']['alter']['more_link'] = 0;
  $handler->display->display_options['fields']['label']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['label']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['label']['alter']['html'] = 0;
  $handler->display->display_options['fields']['label']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['label']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['label']['hide_empty'] = 0;
  $handler->display->display_options['fields']['label']['empty_zero'] = 0;
  $handler->display->display_options['fields']['label']['hide_alter_empty'] = 1;
  /* Field: Party: Party ID */
  $handler->display->display_options['fields']['pid_1']['id'] = 'pid_1';
  $handler->display->display_options['fields']['pid_1']['table'] = 'party';
  $handler->display->display_options['fields']['pid_1']['field'] = 'pid';
  $handler->display->display_options['fields']['pid_1']['relationship'] = 'relation_employee_party';
  $handler->display->display_options['fields']['pid_1']['label'] = '';
  $handler->display->display_options['fields']['pid_1']['exclude'] = TRUE;
  $handler->display->display_options['fields']['pid_1']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['pid_1']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['pid_1']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['pid_1']['alter']['external'] = 0;
  $handler->display->display_options['fields']['pid_1']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['pid_1']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['pid_1']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['pid_1']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['pid_1']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['pid_1']['alter']['more_link'] = 0;
  $handler->display->display_options['fields']['pid_1']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['pid_1']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['pid_1']['alter']['html'] = 0;
  $handler->display->display_options['fields']['pid_1']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['pid_1']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['pid_1']['hide_empty'] = 0;
  $handler->display->display_options['fields']['pid_1']['empty_zero'] = 0;
  $handler->display->display_options['fields']['pid_1']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['pid_1']['separator'] = '';
  $handler->display->display_options['fields']['pid_1']['format_plural'] = 0;
  /* Field: Party: Label */
  $handler->display->display_options['fields']['label_1']['id'] = 'label_1';
  $handler->display->display_options['fields']['label_1']['table'] = 'party';
  $handler->display->display_options['fields']['label_1']['field'] = 'label';
  $handler->display->display_options['fields']['label_1']['relationship'] = 'relation_employee_party';
  $handler->display->display_options['fields']['label_1']['label'] = 'Organization';
  $handler->display->display_options['fields']['label_1']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['label_1']['alter']['make_link'] = 1;
  $handler->display->display_options['fields']['label_1']['alter']['path'] = 'party/[pid_1]';
  $handler->display->display_options['fields']['label_1']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['label_1']['alter']['external'] = 0;
  $handler->display->display_options['fields']['label_1']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['label_1']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['label_1']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['label_1']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['label_1']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['label_1']['alter']['more_link'] = 0;
  $handler->display->display_options['fields']['label_1']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['label_1']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['label_1']['alter']['html'] = 0;
  $handler->display->display_options['fields']['label_1']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['label_1']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['label_1']['hide_empty'] = 0;
  $handler->display->display_options['fields']['label_1']['empty_zero'] = 0;
  $handler->display->display_options['fields']['label_1']['hide_alter_empty'] = 1;
  /* Field: Profile: Email */
  $handler->display->display_options['fields']['party_contact_email']['id'] = 'party_contact_email';
  $handler->display->display_options['fields']['party_contact_email']['table'] = 'field_data_party_contact_email';
  $handler->display->display_options['fields']['party_contact_email']['field'] = 'party_contact_email';
  $handler->display->display_options['fields']['party_contact_email']['relationship'] = 'eid_profile2';
  /* Field: Profile: Phone Number */
  $handler->display->display_options['fields']['party_contact_phone']['id'] = 'party_contact_phone';
  $handler->display->display_options['fields']['party_contact_phone']['table'] = 'field_data_party_contact_phone';
  $handler->display->display_options['fields']['party_contact_phone']['field'] = 'party_contact_phone';
  $handler->display->display_options['fields']['party_contact_phone']['relationship'] = 'eid_profile2';
  $handler->display->display_options['fields']['party_contact_phone']['label'] = 'Phone';
  $handler->display->display_options['fields']['party_contact_phone']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['party_contact_phone']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['party_contact_phone']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['party_contact_phone']['alter']['external'] = 0;
  $handler->display->display_options['fields']['party_contact_phone']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['party_contact_phone']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['party_contact_phone']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['party_contact_phone']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['party_contact_phone']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['party_contact_phone']['alter']['more_link'] = 0;
  $handler->display->display_options['fields']['party_contact_phone']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['party_contact_phone']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['party_contact_phone']['alter']['html'] = 0;
  $handler->display->display_options['fields']['party_contact_phone']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['party_contact_phone']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['party_contact_phone']['hide_empty'] = 0;
  $handler->display->display_options['fields']['party_contact_phone']['empty_zero'] = 0;
  $handler->display->display_options['fields']['party_contact_phone']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['party_contact_phone']['group_rows'] = 1;
  $handler->display->display_options['fields']['party_contact_phone']['delta_offset'] = '0';
  $handler->display->display_options['fields']['party_contact_phone']['delta_reversed'] = 0;
  $handler->display->display_options['fields']['party_contact_phone']['delta_first_last'] = 0;
  $handler->display->display_options['fields']['party_contact_phone']['field_api_classes'] = 0;
  /* Sort criterion: Profile: Name (party_contact_name) */
  $handler->display->display_options['sorts']['party_contact_name_value']['id'] = 'party_contact_name_value';
  $handler->display->display_options['sorts']['party_contact_name_value']['table'] = 'field_data_party_contact_name';
  $handler->display->display_options['sorts']['party_contact_name_value']['field'] = 'party_contact_name_value';
  $handler->display->display_options['sorts']['party_contact_name_value']['relationship'] = 'eid_profile2';
  /* Sort criterion: Profile: Date created */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'profile';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['relationship'] = 'eid_profile2';
  $handler->display->display_options['sorts']['created']['exposed'] = TRUE;
  $handler->display->display_options['sorts']['created']['expose']['label'] = 'Post Date';
  /* Filter criterion: Party: Hats (party_hat) */
  $handler->display->display_options['filters']['party_hat_hat_name']['id'] = 'party_hat_hat_name';
  $handler->display->display_options['filters']['party_hat_hat_name']['table'] = 'field_data_party_hat';
  $handler->display->display_options['filters']['party_hat_hat_name']['field'] = 'party_hat_hat_name';
  $handler->display->display_options['filters']['party_hat_hat_name']['value'] = 'contact';
  $handler->display->display_options['filters']['party_hat_hat_name']['group'] = 1;

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'contacts';
  $handler->display->display_options['menu']['type'] = 'normal';
  $handler->display->display_options['menu']['title'] = 'Contacts';
  $handler->display->display_options['menu']['name'] = 'main-menu';

  $views['contacts'] = $view;

  // Organizations view
  $view = new view;
  $view->name = 'organizations';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'party';
  $view->human_name = 'Organizations';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Organization';
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['query_comment'] = FALSE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '10';
  $handler->display->display_options['style_plugin'] = 'table';
  /* Relationship: Party: Attached Profile */
  $handler->display->display_options['relationships']['eid_profile2']['id'] = 'eid_profile2';
  $handler->display->display_options['relationships']['eid_profile2']['table'] = 'party_attached_entity';
  $handler->display->display_options['relationships']['eid_profile2']['field'] = 'eid_profile2';
  $handler->display->display_options['relationships']['eid_profile2']['label'] = 'Attached Organisation Profile';
  $handler->display->display_options['relationships']['eid_profile2']['required'] = 0;
  $handler->display->display_options['relationships']['eid_profile2']['data_set'] = 'profile2_party_org';
  $handler->display->display_options['relationships']['eid_profile2']['main'] = 0;
  /* Field: Party: Party ID */
  $handler->display->display_options['fields']['pid']['id'] = 'pid';
  $handler->display->display_options['fields']['pid']['table'] = 'party';
  $handler->display->display_options['fields']['pid']['field'] = 'pid';
  $handler->display->display_options['fields']['pid']['label'] = '';
  $handler->display->display_options['fields']['pid']['exclude'] = TRUE;
  $handler->display->display_options['fields']['pid']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['pid']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['pid']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['pid']['alter']['external'] = 0;
  $handler->display->display_options['fields']['pid']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['pid']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['pid']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['pid']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['pid']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['pid']['alter']['more_link'] = 0;
  $handler->display->display_options['fields']['pid']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['pid']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['pid']['alter']['html'] = 0;
  $handler->display->display_options['fields']['pid']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['pid']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['pid']['hide_empty'] = 0;
  $handler->display->display_options['fields']['pid']['empty_zero'] = 0;
  $handler->display->display_options['fields']['pid']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['pid']['separator'] = '';
  $handler->display->display_options['fields']['pid']['format_plural'] = 0;
  /* Field: Party: Label */
  $handler->display->display_options['fields']['label']['id'] = 'label';
  $handler->display->display_options['fields']['label']['table'] = 'party';
  $handler->display->display_options['fields']['label']['field'] = 'label';
  $handler->display->display_options['fields']['label']['label'] = 'Name';
  $handler->display->display_options['fields']['label']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['label']['alter']['make_link'] = 1;
  $handler->display->display_options['fields']['label']['alter']['path'] = 'party/[pid]';
  $handler->display->display_options['fields']['label']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['label']['alter']['external'] = 0;
  $handler->display->display_options['fields']['label']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['label']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['label']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['label']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['label']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['label']['alter']['more_link'] = 0;
  $handler->display->display_options['fields']['label']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['label']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['label']['alter']['html'] = 0;
  $handler->display->display_options['fields']['label']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['label']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['label']['hide_empty'] = 0;
  $handler->display->display_options['fields']['label']['empty_zero'] = 0;
  $handler->display->display_options['fields']['label']['hide_alter_empty'] = 1;
  /* Sort criterion: Profile: Name (party_org_name) */
  $handler->display->display_options['sorts']['party_org_name_value']['id'] = 'party_org_name_value';
  $handler->display->display_options['sorts']['party_org_name_value']['table'] = 'field_data_party_org_name';
  $handler->display->display_options['sorts']['party_org_name_value']['field'] = 'party_org_name_value';
  $handler->display->display_options['sorts']['party_org_name_value']['relationship'] = 'eid_profile2';
  $handler->display->display_options['sorts']['party_org_name_value']['exposed'] = TRUE;
  $handler->display->display_options['sorts']['party_org_name_value']['expose']['label'] = 'Name';
  /* Filter criterion: Party: Hats (party_hat) */
  $handler->display->display_options['filters']['party_hat_hat_name']['id'] = 'party_hat_hat_name';
  $handler->display->display_options['filters']['party_hat_hat_name']['table'] = 'field_data_party_hat';
  $handler->display->display_options['filters']['party_hat_hat_name']['field'] = 'party_hat_hat_name';
  $handler->display->display_options['filters']['party_hat_hat_name']['value'] = 'org';
  /* Filter criterion: Profile: Name (party_org_name) */
  $handler->display->display_options['filters']['party_org_name_value']['id'] = 'party_org_name_value';
  $handler->display->display_options['filters']['party_org_name_value']['table'] = 'field_data_party_org_name';
  $handler->display->display_options['filters']['party_org_name_value']['field'] = 'party_org_name_value';
  $handler->display->display_options['filters']['party_org_name_value']['relationship'] = 'eid_profile2';
  $handler->display->display_options['filters']['party_org_name_value']['exposed'] = TRUE;
  $handler->display->display_options['filters']['party_org_name_value']['expose']['operator_id'] = 'party_org_name_value_op';
  $handler->display->display_options['filters']['party_org_name_value']['expose']['label'] = 'Name';
  $handler->display->display_options['filters']['party_org_name_value']['expose']['operator'] = 'party_org_name_value_op';
  $handler->display->display_options['filters']['party_org_name_value']['expose']['identifier'] = 'party_org_name_value';
  $handler->display->display_options['filters']['party_org_name_value']['expose']['required'] = 0;
  $handler->display->display_options['filters']['party_org_name_value']['expose']['multiple'] = FALSE;

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'organizations';
  $handler->display->display_options['menu']['type'] = 'normal';
  $handler->display->display_options['menu']['title'] = 'Organizations';
  $handler->display->display_options['menu']['name'] = 'main-menu';

  $views['organizations'] = $view;

  return $views;
}