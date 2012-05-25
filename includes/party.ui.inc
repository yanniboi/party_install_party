<?php

/**
 * @file
 * Provides a controller for building a party overview form.
 */

/**
 * Controller for providing Party UI.
 */
class PartyUIController extends EntityDefaultUIController {

  /**
   * Builds the entity overview form.
   */
  public function overviewForm($form, &$form_state) {
    // Show an overview for all entities.
    $form['actions'] = $this->actionsForm();
    $form['table'] = $this->overviewTable();
    $form['pager'] = array('#theme' => 'pager');
    return $form;
  }

  /**
   * Builds the actions form.
   *
   * @todo: figure out clean parameters here!
   */
  public function actionsForm() {
    // This gives us the bulk operations form element
    $form['options'] = array(
      '#type' => 'fieldset',
      '#title' => t('Update options'),
      '#attributes' => array('class' => array('container-inline')),
    );
    $options = array();
    // Get party operations from any modules that implement hook_party_operations().
    foreach (module_invoke_all('party_operations') as $operation => $array) {
      $options[$operation] = $array['label'];
    }
    $form['options']['operation'] = array(
      '#type' => 'select',
      '#title' => t('Operation'),
      '#title_display' => 'invisible',
      '#options' => $options,
      '#default_value' => 'merge',
    );
    $options = array();
    $form['options']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Update'),
    );

    return $form;
  }

}
