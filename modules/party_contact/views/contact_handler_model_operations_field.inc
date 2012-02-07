<?php

/**
 * This field handler aggregates operations that can be done on a contact
 * under a single field providing a more flexible way to present them in a view
 */
class contact_handler_contact_operations_field extends views_handler_field {
  function construct() {
    parent::construct();

    $this->additional_fields['contact_id'] = 'contact_id';
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function render($values) {

    $links = menu_contextual_links('contact', 'admin/content/contacts/contact', array($this->get_value($values, 'contact_id')));
    
    if (!empty($links)) {
      return theme('links', array('links' => $links, 'attributes' => array('class' => array('links', 'inline', 'operations'))));
    }
  }
}
