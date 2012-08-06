<?php

/**
 * @file
 * Admin page callbacks for the Yandex.Webmaster module.
 *
 * @ingroup yandex_webmaster
 */

/**
 * Load idna_convert class for correct work with domain-names in punycode.
 */
require_once('idna_convert.class.php');

/**
 * Display configuration page for the Yandex.Webmaster module.
 *
 * @ingroup yandex_webmaster
 */
function yandex_webmaster_config_form($form, $form_state) {
  drupal_add_css(drupal_get_path('module', 'yandex_webmaster') . '/yandex_webmaster.css');
  drupal_add_js(drupal_get_path('module', 'yandex_webmaster') . '/yandex_webmaster.js');

  $client = yandex_webmaster_client();
  $form = array();
  // Check access token for expiration.
  yandex_webmaster_check_token_expiration();

  $form['branding'] = yandex_webmaster_branding_item(TRUE);

  // Parse host list.
  $hostlist = yandex_webmaster_get_hosts();
  yandex_webmaster_update_stats($hostlist);
  // Check for current host.
  $idna = new idna_convert();
  $current_host = $idna->decode($_SERVER['HTTP_HOST']);
  $current_host_id = yandex_webmaster_get_host_id($hostlist);

  // Security code field.
  $code = variable_get('yandex_webmaster_code', '');
  $form['code_field'] = array(
    '#title' => t('Security code'),
    '#type' => 'item',
  );
  $form['code_field']['yandex_webmaster_code'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('yandex_webmaster_code', ''),
    '#size' => 8,
    '#maxlength' => 8,
    '#prefix' => '<div class="container-inline form-item">',
  );
  $form['code_field']['button'] = array(
    '#type' => 'button',
    '#value' => $code != '' ? t('Refresh') : t('Get code'),
    '#suffix' => '</div>',
    '#attributes' => array(
      'class' => array('modal'),
      'rel' => yandex_webmaster_domain_name('oauth') . 'authorize?response_type=code&wizard=drupal7_yandex_webmaster_dylan_russain&client_id=' . $client->id . '&display=iframe',
      'onclick' => 'return false;',
    ),
  );

  // Statistics block.
  $form['yandex_webmaster_block_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable statistics block'),
    '#default_value' => variable_get('yandex_webmaster_block_enabled', TRUE),
  );

  // Display all hosts.
  $form['yandex_webmaster_full_list'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display full host list'),
    '#default_value' => variable_get('yandex_webmaster_full_list', TRUE),
  );

  // Select language and domain name.
  $form['yandex_webmaster_domain'] = array(
    '#type' => 'select',
    '#title' => t('Select Yandex.Webmaster domain name'),
    '#options' => array(
      'com' => t('Default') . ' (webmaster.yandex.com)',
      'ru' => t('Russian') . ' (webmaster.yandex.ru)',
      'ua' => t('Ukrainian') . ' (webmaster.yandex.ua)',
    ),
    '#default_value' => variable_get('yandex_webmaster_domain', 'com'),
  );
  

  // Don't display other fields, if security code is empty.
  if (variable_get('yandex_webmaster_token', '') == '') {
    return system_settings_form($form);
  }
  // Empty host list.
  if (!count($hostlist)) {
    return system_settings_form($form);
  }
  // Check for failed verification and sites in subfolders.
  if ($current_host_id != -1 && $hostlist[$current_host_id]['verification'] == 'VERIFICATION_FAILED') {
    $uin = yandex_webmaster_get_host_uin($current_host_id);
    // Get uin for current host.
    if (!$uin) {
      drupal_set_message(t('An error occurred while getting current host UIN from Yandex.Webmaster service.'), 'error');
    }
    else {
      if (base_path() != '/') {
        $form['manual_verification_message'] = array(
          '#type' => 'item',
          '#markup' => '<div class="messages warning">'
            . t('Current site (@site) can\'t be automatic verified. You will need to create empty text file "@filename" in site root directory.', array('@site' => $current_host, '@filename' => 'yandex_' . $uin . '.txt'))
            . '</div>',
        );
      }
      elseif (variable_get('yandex_webmaster_verification_type', '') == 'META_TAG') {
        $form['manual_verification_message'] = array(
          '#type' => 'item',
          '#markup' => '<div class="messages warning">'
            . t('Unfortunately, Yandex.Webmaster service may not process meta-tag verification for the first time. To run check procedure once again, !link.', array('!link' => l(t('click here'), 'http://webmaster.yandex.com/site/verification.xml?wizard=verification&wizard-hostid=' . $current_host_id . '&host=' . $current_host_id, array(
              'attributes' => array(
                'target' => '_blank',
                'title' => t('Open in new window'),
              ),
              ))))
            . '</div>',
        );
      }
    }
  }

  // Display hosts list.
  $basic_host = variable_get('yandex_webmaster_basic_host_id', 0);
  $header = array(
    'hostname' => t('Host name'),
    'tcy' => t('TCY'),
    'url_count' => t('URL count'),
    'index_count' => t('Index count'),
    'last_access' => t('Last access'),
    'actions' => t('Actions'),
  );
  $options = array();
  $default_value = array();

  // Fill rows.
  $is_full_list = variable_get('yandex_webmaster_full_list', TRUE);
  foreach ($hostlist as $host_id => $host) {
    $actions = '';
    if ($host['url-count'] > 0) {
      $actions .= l(t('Open statistics'), 'admin/reports/yandex_webmaster/' . $host_id, array(
        'attributes' => array(
          /* 'class' => 'yandex_webmaster_chart', */
          'class' => 'yandex_webmaster_actions',
          'title' => t('Open statistics'),
        ),
      ));
    }
    if ($host_id == $current_host_id) {
      $actions .= l(t('Remove site'), 'admin/config/yandex_webmaster/remove', array(
        'attributes' => array(
          /* 'class' => 'yandex_webmaster_remove', */
          'class' => 'yandex_webmaster_actions',
          'title' => t('Remove site'),
        ),
      ));
    }
    if (!$is_full_list && $host_id != $current_host_id && $host_id != $basic_host) {
      continue;
    }

    // Get right hostname.
    $hostname = $host['name'];
    $idna = new idna_convert();
    $url = $idna->encode('http://' . $host['name']);
    // Display host state as icon.
    $state = $host['verification'];
    $virused = $host['virused'] == 'false' ? '' : ' VIRUSED';

    $status = yandex_webmaster_state_description($state, 'verification') . ' ' . yandex_webmaster_state_description($host['crawling'], 'index');
    $options[$host_id] = array(
      '#attributes' => array(
        'class' => array('yandex_webmaster_host_row' . $virused),
      ),
      'hostname' => '<div class="yandex_webmaster_host_state ' . $state . $virused . '" title="' . $status . '">'
        . '<div class="yandex_webmaster_hostname">' . $hostname
        . l($hostname, $url, array(
          'attributes' => array(
            'class' => 'yandex_webmaster_external_link',
            'target' => '_blank',
            'title' => t('Open in new window'),
          ),
        ))
        . '<br />'
        . '<span>'
        . $status
        . '</span> '
        . '</div>',
      'tcy' => $host['tcy'],
      'url_count' => $host['url-count'] > 0 ? number_format($host['url-count'], 0, '', ' ') : $host['url-count'],
      'index_count' => $host['index-count'] > 0 ? number_format($host['index-count'], 0, '', ' ') : $host['index-count'],
      'last_access' => '<div class="yandex_webmaster_last_access">'
        . ($host['last-access'] > 0 ? t('@time ago', array('@time' => format_interval(time() - $host['last-access']))) . '<br /><span>' . format_date($host['last-access']) . '</span>' : $host['last-access'])
        . '</div>',
      'actions' => $actions,
    );
  }
  // Add current host to list, if it has not been added.
  if ($current_host_id == -1) {
    $options[$current_host] = array(
      'hostname' => '<div class="yandex_webmaster_host_state WAITING" title="' . t('Not in your host list. Need to be added.') . '">'
        . '<div class="yandex_webmaster_hostname">' . $current_host . '<br />'
        . '<span>' . t('Not in your host list. Need to be added.') . '</span> '
        . '</div>',
      'tcy' => array(),
      'url_count' => array(),
      'index_count' => array(),
      'last_access' => array(),
      'actions' => array(
        'data' => array(
          '#type' => 'link',
          '#title' => t('Add site'),
          '#href' => 'admin/config/yandex_webmaster/add',
          '#attributes' => array(
            'class' => array('yandex_webmaster_add_site'),
          ),
        ),
      ),
    );
  }
  // Display host list.
  $form['yandex_webmaster_basic_host_id'] = array(
    '#title' => 'Host list',
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#default_value' => $basic_host,
    '#multiple' => FALSE,
    '#empty' => t('Not one host was previously added.'),
  );
  return system_settings_form($form);
}

/**
 * Validate yandex_webmaster_config_form form submissions and save access token,
 * if security code was changed.
 */
function yandex_webmaster_config_form_validate($form, &$form_state) {
  // Send request only when security code was changed.
  if ($form_state['values']['yandex_webmaster_code'] != variable_get('yandex_webmaster_code', '')) {
    $client = yandex_webmaster_client();
    $options = array(
      'method' => 'POST',
      'data' => 'grant_type=authorization_code&code=' . $form_state['values']['yandex_webmaster_code'] . '&client_id=' . $client->id . '&client_secret=' . $client->secret,
      'timeout' => 15,
      'headers' => array('Content-Type' => 'application/x-www-form-urlencoded'),
    );
    $result = drupal_http_request(yandex_webmaster_domain_name('oauth') . 'token', $options);
    if ($result->code == '200') {
      $json = drupal_json_decode($result->data);
      // Save access token and his expiration date.
      variable_set('yandex_webmaster_token', $json['access_token']);
      variable_set('yandex_webmaster_token_expiration', time() + (int) $json['expires_in']);
      drupal_set_message(t('Security code was successfully checked.'));
      // Get Yandex.API user ID.
      $uid = yandex_webmaster_get_user_id();
      if ($uid != -1) {
        variable_set('yandex_webmaster_uid', $uid);
      }
    }
    else {
      // Clear previous settings.
      variable_set('yandex_webmaster_uid', '');
      variable_set('yandex_webmaster_basic_host_id', 0);
      variable_set('yandex_webmaster_token', '');
      variable_set('yandex_webmaster_token_expiration', '');
      // Display error message with description.
      $error = '';
      if (isset($result->error)) {
        $error = $result->error;
      }
      elseif (isset($result->status_message)) {
        $error = yandex_webmaster_state_description($result->status_message);
      }
      drupal_set_message(t('Access request return error: @error. Try to get a new security code.', array('@error' => $result->code . ' (' . $error . ')')), 'error');
    }
  }
  // Rebuild menu when default site was changed.
  if (isset($form_state['values']['yandex_webmaster_basic_host_id']) && $form_state['values']['yandex_webmaster_basic_host_id'] != variable_get('yandex_webmaster_basic_host_id', '')) {
    menu_rebuild();
  }
}

/**
 * Display page for adding current site to Yandex.Webmaster service.
 *
 * @ingroup yandex_webmaster
 */
function yandex_webmaster_add_site_form($form, $form_state) {
  if (variable_get('yandex_webmaster_code', '') == '') {
    drupal_set_message(t('Before adding new site to Yandex.Webmaster, you need to enter security code.'));
    drupal_goto('admin/config/services/yandex_webmaster');
  }
  // Verification type.
  $form['yandex_webmaster_use_metatag'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use meta-tag on the front page for verification'),
    '#disabled' => yandex_webmaster_get_host_id(yandex_webmaster_get_hosts()) != -1,
    '#default_value' => FALSE,
  );
  $form['actions'] = array(
    '#type' => 'tree',
    'submit' => array(
      '#type' => 'button',
      '#value' => t('Add site'),
    ),
  );

  return $form;
}

/**
 * Validate yandex_webmaster_add_site_form form submissions.
 */
function yandex_webmaster_add_site_form_validate($form, &$form_state) {
  $uid = variable_get('yandex_webmaster_uid', '');
  $idna = new idna_convert();
  // Now we will encode hostname.
  $host = $idna->encode($_SERVER['HTTP_HOST']);

  // Send server request.
  $options = array(
    'method' => 'POST',
    'headers' => array('Authorization' => 'OAuth ' . variable_get('yandex_webmaster_token', '')),
    'data' => '<host><name>' . $host . '</name></host>',
  );
  $result = drupal_http_request(yandex_webmaster_domain_name() . 'api/' . $uid . '/hosts', $options);
  // Host has sucessfully added to Yandex.Webmaster.
  if ($result->code == 201) {
    drupal_set_message(t('Site was sucessfully added to Yandex.Webmaster.'));
  }
  // If an error has occurred.
  else {
    preg_match("/<error>(.*?)<\/error>/", $result->data, $matches);
    $error = $matches[1];
    drupal_set_message(t('An error occurred while adding new site to Yandex.Webmaster. @error', array('@error' => $error)), 'error');
  }

  // Get current host id
  $hostlist = yandex_webmaster_get_hosts();
  // Empty host list.
  if (!count($hostlist)) {
    drupal_set_message(t('An error occurred while getting current host ID from Yandex.Webmaster service.'), 'error');
    drupal_goto('admin/config/services/yandex_webmaster');
  }
  $host_id = '';
  // Now we will decode hostname.
  $host = $idna->decode($_SERVER['HTTP_HOST']);
  foreach ($hostlist as $id => $item) {
    if ($item['name'] == $host || str_replace('www.', '', $host) == str_replace('www.', '', $item['name'])) {
      $host_id = $id;
      break;
    }
  }
  if ($host_id == '') {
    drupal_set_message(t('An error occurred while getting current host ID from Yandex.Webmaster service.'), 'error');
    drupal_goto('admin/config/services/yandex_webmaster');
  }
  variable_set('yandex_webmaster_basic_host_id', $host_id);

  // Get uin to add new site.
  $uin = yandex_webmaster_get_host_uin($host_id);
  if (!$uin) {
    drupal_set_message(t('An error occurred while getting current host UIN from Yandex.Webmaster service.'), 'error');
    drupal_goto('admin/config/services/yandex_webmaster');
  }

  // Check file access permission to create new file in site root directory
  // for verification.
  $root = $_SERVER['DOCUMENT_ROOT'];
  $use_meta_tag = $form_state['values']['yandex_webmaster_use_metatag'];
  if (!$use_meta_tag) {
    // Create empty file and set verification type.
    $use_meta_tag = (!is_writable($root) || file_put_contents($root . 'yandex_' . $uin . '.txt', '') === FALSE);
  }
  // Drupal installation in subfolder.
  if (base_path() != '/') {
    drupal_set_message(t('Current site (@site) can\'t be automatic verified. You will need to create empty text file "@filename" in site root directory.', array('@site' => $current_host, '@filename' => 'yandex_' . $uin . '.txt')), 'error');
    $use_meta_tag = FALSE;
  }

  // Set verification type code.
  $verification_type = $use_meta_tag ? 'META_TAG' : 'TXT_FILE';
  variable_set('yandex_webmaster_verification_type', $verification_type);
  // Send PUT-request
  $options = array(
    'method' => 'PUT',
    'headers' => array('Authorization' => 'OAuth ' . variable_get('yandex_webmaster_token', '')),
    'data' => '<host><type>' . $verification_type . '</type></host>',
  );
  $result = drupal_http_request(yandex_webmaster_domain_name() . 'api/' . $uid . '/hosts/' . $host_id . '/verify', $options);
  if ($result->code != 204) {
    drupal_set_message(t('An error occurred while sending verification type to Yandex.Webmaster service.'), 'error');
  }

  menu_rebuild();
  drupal_goto('admin/config/services/yandex_webmaster');
}

/**
 * Remove current site from Yandex.Webmaster service.
 *
 * @ingroup yandex_webmaster
 */
function yandex_webmaster_remove_site_form($form, $form_state) {
  $form['text'] = array(
    '#type' => 'item',
    '#markup' => t('Are you sure want to remove current site from Yandex.Webmaster service?'),
  );
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Remove'),
  );
  $form['actions']['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
  );
  return $form;
}

/**
 * Submit procedure for removing current site from Yandex.Webmaster service.
 */
function yandex_webmaster_remove_site_form_submit($form, &$form_state) {
  if ($form_state['values']['op'] != t('Remove')) {
    drupal_goto('admin/config/services/yandex_webmaster');
  }
  if (variable_get('yandex_webmaster_code', '') == '') {
    drupal_set_message(t('Before adding new site to Yandex.Webmaster, you need to enter security code.'));
    drupal_goto('admin/config/services/yandex_webmaster');
  }

  $uid = variable_get('yandex_webmaster_uid', '');
  $idna = new idna_convert();
  $host = $idna->encode($_SERVER['HTTP_HOST']);

  $hostlist = yandex_webmaster_get_hosts();
  // Empty host list.
  if (!count($hostlist)) {
    drupal_set_message(t('An error occurred while sending delete request to Yandex.Webmaster service.'), 'error');
    drupal_goto('admin/config/services/yandex_webmaster');
  }
  $current_host_id = yandex_webmaster_get_host_id($hostlist);
  // Send DELETE-request.
  $options = array(
    'method' => 'DELETE',
    'headers' => array('Authorization' => 'OAuth ' . variable_get('yandex_webmaster_token', '')),
  );
  $result = drupal_http_request(yandex_webmaster_domain_name() . 'api/' . $uid . '/hosts/' . $current_host_id, $options);
  if ($result->code == 204) {
    drupal_set_message(t('Site was sucessfully removed from Yandex.Webmaster service.'));
  }
  else {
    drupal_set_message(t('An error occurred while sending delete request to Yandex.Webmaster service.'), 'error');
  }
  variable_set('yandex_webmaster_basic_host_id', 0);

  drupal_goto('admin/config/services/yandex_webmaster');
  return array();
}