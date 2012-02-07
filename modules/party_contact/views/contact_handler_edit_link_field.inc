<?php

/**
 * @file
 * Contains a Views field handler to take care of displaying edit links
 * as fields
 */


class contact_handler_edit_link_field extends contact_handler_link_field {
  function construct() {
    parent::construct();
    $this->additional_fields['type'] = 'type';
  }


  function render($values) {
    $type = $values->{$this->aliases['type']};
    
    //Creating a dummy contact to check access against
    $dummy_contact = (object) array('type' => $type);
    if (!contact_access('edit', $dummy_contact)) {
      return;
    }
    
    $text = !empty($this->options['text']) ? $this->options['text'] : t('edit');
    $contact_id = $values->{$this->aliases['contact_id']};
    
    return l($text, 'admin/content/contacts/contact/' . $contact_id . '/edit');
  }
}
