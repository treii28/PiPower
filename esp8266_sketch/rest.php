<?php

/*
 * a dummy rest test script for talking to/from the remote that pretends to be a finished
 *  RESTful json server on the ESP8266.
 */
 
header('Content-Type: application/json');

$input = json_decode(file_get_contents("php://input"), true);
$post =
 (isset($input['func']) ? $input :
  (isset($_POST['func']) ? $_POST :
   (isset($_GET['func']) ? $_GET :
    (isset($_REQUEST['func']) ? $_REQUEST : array())
   )
  )
 );

$data = array();

  $debug = array(
    "message" => "parameters do not include a func",
    "post" => $post,
    "php_input" => $input,
    "_POST" => $_POST,
    "_GET" => $_GET,
    "_REQUEST" => $_REQUEST,
//    "GLOBALS" => $GLOBALS,
    "_ENV" => $_ENV,
    "_SERVER" => $_SERVER
  );
$debug_json = "Input:\n" . json_encode($debug, JSON_PRETTY_PRINT);

$relays = array(
  array(
    "pin" => 1,
    "name" => "Rasp1",
    "id" => "p1",
    "status" => 1,
    "address" => "192.168.1.101"
  ),
  array(
    "pin" => 2,
    "name" => "Rasp2",
    "id" => "p2",
    "status" => 0,
    "address" => "192.168.1.102"
  )
);

if(isset($post['func'])) {
  $func = $post['func'];
  $data['func'] = $func;
  $debug_json .= "\n\nfunc: " . $func . "\n";
  switch ($func) {
    case "toggle":
      if(isset($post['pin'])) {
        if(is_numeric($post['pin']) && (intval($post['pin']) >= 0)) {
          $pin = intval($post['pin']);
          $status = $relays[$pin-1]['status'];
          $data['pin'] = $pin;
          $data['state'] = 1 - $status;
        } else {
          $data['error'] = "invalid value for pin";
        }
      } else {
        $data['error'] = "no pin specified";
      }
      break;
    case "info":
      } else {
        $data['error'] = "no pin specified";
      }
      break;
    case "info":
      $data['relays'] = $relays;
      break;
    default:
      $data['error'] = "unknown function";
      break;
  }
} else {
  $data['error'] = "function not specified";
}

$data_json = json_encode((object)$data);

$debug_json .= "\n\nOutput:\n" . $data_json . "\n";
file_put_contents("/tmp/rest_debug.txt", $debug_json);

echo $data_json;
