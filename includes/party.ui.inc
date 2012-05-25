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

  /**
   * Generates the table headers for the overview table.
   */
  protected function overviewTableHeaders($conditions, $rows, $additional_header = array()) {
    $header = $additional_header;
    array_unshift($header, t('Id'), t('Label'));
    if (!empty($this->entityInfo['exportable'])) {
      $header[] = t('Status');
    }

    $header[] = t('Type');

    // Add operations with the right colspan.
    $header[] = array('data' => t('Operations'), 'colspan' => $this->operationCount());
    return $header;
  }

  /**
   * Generates the row for the passed entity and may be overridden in order to
   * customize the rows.
   */
  protected function overviewTableRow($conditions, $id, $party, $additional_cols = array()) {
    // Get the basic row from the parent class.
    $row = parent::overviewTableRow($conditions, $id, $party, $additional_cols);

    // Take off the label so we can rearrange things.
    $label = array_shift($row);

    array_unshift($row,
      $id,
      $label,
      $this->entityInfo['bundles'][$party->type]['label']
    );

    return $row;
  }

}
