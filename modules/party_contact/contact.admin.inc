<?php

/**
 * @file
 * Contact editing UI.
 *
 * We make very little use of the EntityAPI interface for this - preferring instead to use
 * views. That offers more flexibility to change a UI that will, more often than not,
 * be end-user facing.
 */

/**
 * UI controller.
 */
class ContactUIController extends EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults. Main reason for doing this is that
   * parent class hook_menu() is optimized for entity type administration.
   */
  public function hook_menu() {
    
    $items = array();
    $id_count = count(explode('/', $this->path));
    $wildcard = isset($this->entityInfo['admin ui']['menu wildcard']) ? $this->entityInfo['admin ui']['menu wildcard'] : '%' . $this->entityType;

    $items[$this->path] = array(
      'title' => 'Contacts',
      'description' => 'Add edit and update contacts.',
      'page callback' => 'system_admin_menu_block_page',
      'access arguments' => array('access administration pages'),
      'file path' => drupal_get_path('module', 'system'),
      'file' => 'system.admin.inc',
    );
    
    // Change the overview menu type for the list of contacts.
    $items[$this->path]['type'] = MENU_LOCAL_TASK;
    
    // Change the add page menu to multiple types of entities
    $items[$this->path . '/add'] = array(
      'title' => 'Add a contact',
      'description' => 'Add a new contact',
      'page callback'  => 'contact_add_page',
      'access callback'  => 'contact_access',
      'access arguments' => array('edit'),
      'type' => MENU_NORMAL_ITEM,
      'weight' => 20,
      'file' => 'contact.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module'])

    );
    
    // Add menu items to add each different type of entity.
    foreach (contact_get_types() as $type) {
      $items[$this->path . '/add/' . $type->type] = array(
        'title' => 'Add ' . $type->label,
        'page callback' => 'contact_form_wrapper',
        'page arguments' => array(contact_create(array('type' => $type->type))),
        'access callback' => 'contact_access',
        'access arguments' => array('edit', 'edit ' . $type->type),
        'file' => 'contact.admin.inc',
        'file path' => drupal_get_path('module', $this->entityInfo['module'])
      );
    }

    // Loading and editing contact entities
    $items[$this->path . '/contact/' . $wildcard] = array(
      'page callback' => 'contact_form_wrapper',
      'page arguments' => array($id_count + 1),
      'access callback' => 'contact_access',
      'access arguments' => array('edit', $id_count + 1),
      'weight' => 0,
      'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
      'file' => 'contact.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module'])
    );
    $items[$this->path . '/contact/' . $wildcard . '/edit'] = array(
      'title' => 'Edit',
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => -10,
      'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
    );
    
    $items[$this->path . '/contact/' . $wildcard . '/delete'] = array(
      'title' => 'Delete',
      'page callback' => 'contact_delete_form_wrapper',
      'page arguments' => array($id_count + 1),
      'access callback' => 'contact_access',
      'access arguments' => array('edit', $id_count + 1),
      'type' => MENU_LOCAL_TASK,
      'context' => MENU_CONTEXT_INLINE,
      'weight' => 10,
      'file' => 'contact.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module'])
    );
    
    // Menu item for viewing contacts
    $items['contact/' . $wildcard] = array(
      //'title' => 'Title',
      'title callback' => 'contact_page_title',
      'title arguments' => array(1),
      'page callback' => 'contact_page_view',
      'page arguments' => array(1),
      'access callback' => 'contact_access',
      'access arguments' => array('view', 1),
      'type' => MENU_CALLBACK,
    );
    return $items;
  }
  
  
  /**
   * Create the markup for the add Contact Entities page within the class
   * so it can easily be extended/overriden.
   */ 
  public function addPage() {
    $item = menu_get_item();
    $content = system_admin_menu_block($item);

    if (count($content) == 1) {
      $item = array_shift($content);
      drupal_goto($item['href']);
    }    
        
    return theme('contact_add_list', array('content' => $content));
  }
  
}


/**
 * Form callback wrapper: create or edit a contact.
 *
 * @param $contact
 *   The contact object being edited by this form.
 *
 * @see contact_edit_form()
 */
function contact_form_wrapper($contact) {
  // Add the breadcrumb for the form's location.
  contact_set_breadcrumb();
  return drupal_get_form('contact_edit_form', $contact);
}


/**
 * Form callback wrapper: delete a contact.
 *
 * @param $contact
 *   The contact object being edited by this form.
 *
 * @see contact_edit_form()
 */
function contact_delete_form_wrapper($contact) {
  // Add the breadcrumb for the form's location.
  //contact_set_breadcrumb();
  return drupal_get_form('contact_delete_form', $contact);
}


/**
 * Form callback: create or edit a contact.
 *
 * @param $contact
 *   The contact object to edit or for a create form an empty contact object
 *     with only a contact type defined.
 */
function contact_edit_form($form, &$form_state, $contact) {
  // Add the default field elements.
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Contact Name'),
    '#default_value' => isset($contact->name) ? $contact->name : '',
    '#maxlength' => 255,
    '#required' => TRUE,
    '#weight' => -5,
  );
  
  $form['data']['#tree'] = TRUE;
  $form['data']['sample_data'] = array(
    '#type' => 'checkbox',
    '#title' => t('An interesting contact switch'),
    '#default_value' => isset($contact->data['sample_data']) ? $contact->data['sample_data'] : 1,
  );

  // Add the field related form elements.
  $form_state['contact'] = $contact;
  field_attach_form('contact', $contact, $form, $form_state);

  $form['actions'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('form-actions')),
    '#weight' => 400,
  );

  // We add the form's #submit array to this button along with the actual submit
  // handler to preserve any submit handlers added by a form callback_wrapper.
  $submit = array();

  if (!empty($form['#submit'])) {
    $submit += $form['#submit'];
  }

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save contact'),
    '#submit' => $submit + array('contact_edit_form_submit'),
  );
  
  if (!empty($contact->name)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete contact'),
      '#suffix' => l(t('Cancel'), 'admin/content/contacts'),
      '#submit' => $submit + array('contact_form_submit_delete'),
      '#weight' => 45,
    );
  }

  // We append the validate handler to #validate in case a form callback_wrapper
  // is used to add validate handlers earlier.
  $form['#validate'][] = 'contact_edit_form_validate';
  return $form;
}


/**
 * Form API validate callback for the contact form
 */
function contact_edit_form_validate(&$form, &$form_state) {
  $contact = $form_state['contact'];
  
  // Notify field widgets to validate their data.
  field_attach_form_validate('contact', $contact, $form, $form_state);
}


/**
 * Form API submit callback for the contact form.
 * 
 * @todo remove hard-coded link
 */
function contact_edit_form_submit(&$form, &$form_state) {
  
  $contact = entity_ui_controller('contact')->entityFormSubmitBuildEntity($form, $form_state);
  // Save the contact and go back to the list of contacts
  
  // Add in created and changed times.
  if ($contact->is_new = isset($contact->is_new) ? $contact->is_new : 0){
    $contact->created = time();
  }

  $contact->changed = time();
  
  $contact->save();
  $form_state['redirect'] = 'admin/content/contacts';
}

/**
 * Form API submit callback for the delete button.
 * 
 * @todo Remove hard-coded path
 */
function contact_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/content/contacts/contact/' . $form_state['contact']->contact_id . '/delete';
}


/**
 * Form callback: confirmation form for deleting a contact.
 *
 * @param $contact
 *   The contact to delete
 *
 * @see confirm_form()
 */
function contact_delete_form($form, &$form_state, $contact) {
  $form_state['contact'] = $contact;

  $form['#submit'][] = 'contact_delete_form_submit';

  $form = confirm_form($form,
    t('Are you sure you want to delete contact %name?', array('%name' => $contact->name)),
    'admin/content/contacts/contact',
    '<p>' . t('This action cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );
  
  return $form;
}

/**
 * Submit callback for contact_delete_form
 */
function contact_delete_form_submit($form, &$form_state) {
  $contact = $form_state['contact'];

  contact_delete($contact);

  drupal_set_message(t('The contact %name has been deleted.', array('%name' => $contact->name)));
  watchdog('contact', 'Deleted contact %name.', array('%name' => $contact->name));

  $form_state['redirect'] = 'admin/content/contacts';
}



/**
 * Page to add Contact Entities.
 *
 * @todo Pass this through a proper theme function
 */
function contact_add_page() {
  $controller = entity_ui_controller('contact');
  return $controller->addPage();
}


/**
 * Displays the list of available contact types for contact creation.
 *
 * @ingroup themeable
 */
function theme_contact_add_list($variables) {
  $content = $variables['content'];
  $output = '';
  if ($content) {
    $output = '<dl class="contact-type-list">';
    foreach ($content as $item) {
      $output .= '<dt>' . l($item['title'], $item['href']) . '</dt>';
      $output .= '<dd>' . filter_xss_admin($item['description']) . '</dd>';
    }
    $output .= '</dl>';
  }
  else {
    if (user_access('administer contact types')) {
      $output = '<p>' . t('Contact Entities cannot be added because you have not created any contact types yet. Go to the <a href="@create-contact-type">contact type creation page</a> to add a new contact type.', array('@create-contact-type' => url('admin/structure/contact_types/add'))) . '</p>';
    }
    else {
      $output = '<p>' . t('No contact types have been created yet for you to use.') . '</p>';
    }
  }

  return $output;
}





/**
 * Sets the breadcrumb for administrative contact pages.
 */
function contact_set_breadcrumb() {
  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('Administration'), 'admin'),
    l(t('Content'), 'admin/content'),
    l(t('Contacts'), 'admin/content/contacts'),
  );

  drupal_set_breadcrumb($breadcrumb);
}



