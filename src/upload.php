<?php
$mysqli = new mysqli("p:83.212.127.86", "MIS", "#MIS2018", "podatki");
if (mysqli_connect_errno()) {
    printf("Database connect failed: %s\n", mysqli_connect_error());
    exit();
}

if(isset($_GET["id"], $_GET["flags"], $_GET["volume"])) {
    $idEscaped = $mysqli->real_escape_string($_GET["id"]);
    $flagsEscaped = $mysqli->real_escape_string($_GET["flags"]);
    $volumeEscaped = $mysqli->real_escape_string($_GET["volume"]);
    if (!$mysqli->query("INSERT into vsipodatki (id_naprave, error_flag, volumen) VALUES ('$idEscaped', '$flagsEscaped', '$volumeEscaped')")) {
        printf("Error: %s\n", $mysqli->sqlstate);
    }
}

?>