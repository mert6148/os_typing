<?php
$secure = (!empty($_SERVER['HTTPS']) && $_SERVER['HTTPS'] !== 'off') || (!empty($_SERVER['SERVER_PORT']) && (int)$_SERVER['SERVER_PORT'] === 443);
session_set_cookie_params([
    'lifetime' => 0,
    'path' => '/',
    'domain' => $_SERVER['HTTP_HOST'] ?? '',
    'secure' => $secure,
    'httponly' => true,
    'samesite' => 'Strict',
]);

session_start();
require_once __DIR__ . '/db_security.php';

use App\Database\Config\DBSecurity;

// Simple CSRF token helper (demo)
if (empty($_SESSION['csrf_token']) || !is_string($_SESSION['csrf_token'])) {
    $_SESSION['csrf_token'] = bin2hex(random_bytes(16));
}

$errors = [];
$osName = 'Linux';
$osVersion = '5.11.0-27-generic';

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $csrf = (string) (filter_input(INPUT_POST, 'csrf', FILTER_UNSAFE_RAW) ?? '');
    if (!hash_equals((string)($_SESSION['csrf_token'] ?? ''), $csrf)) {
        $errors[] = 'Invalid CSRF token.';
    } else {
        // Use raw input then validate against a conservative whitelist
        $rawName = trim((string) (filter_input(INPUT_POST, 'os_name', FILTER_UNSAFE_RAW) ?? ''));
        $rawVersion = trim((string) (filter_input(INPUT_POST, 'os_version', FILTER_UNSAFE_RAW) ?? ''));

        // Allow letters, numbers, whitespace, dot, dash, underscore (international letters allowed)
        if (!preg_match('/^[\p{L}0-9\s._-]{1,128}$/u', $rawName)) {
            $errors[] = 'OS name contains invalid characters.';
        } else {
            $osName = htmlspecialchars(mb_substr($rawName, 0, 128), ENT_QUOTES | ENT_SUBSTITUTE, 'UTF-8');
        }

        if (!preg_match('/^[\p{L}0-9\s._+\-]{1,128}$/u', $rawVersion)) {
            $errors[] = 'OS version contains invalid characters.';
        } else {
            $osVersion = htmlspecialchars(mb_substr($rawVersion, 0, 128), ENT_QUOTES | ENT_SUBSTITUTE, 'UTF-8');
        }
    }
}
?>
<!doctype html>
<html lang="tr">
<head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>OS Typing Demo</title>
    <link rel="stylesheet" href="styles.css">
    <meta http-equiv="Content-Security-Policy" content="default-src 'self'; style-src 'self' 'unsafe-inline';">
</head>
<body>
    <div class="container">
        <h1>OS Typing Demo</h1>
        <?php if ($errors): ?>
            <div class="error">
                <ul>
                    <?php foreach ($errors as $e): ?>
                        <li><?= htmlspecialchars($e) ?></li>
                    <?php endforeach; ?>
                </ul>
            </div>
        <?php endif; ?>

        <section class="card">
            <h2>Current OS Info</h2>
            <p><strong>Operating System:</strong> <?= $osName ?></p>
            <p><strong>Version:</strong> <?= $osVersion ?></p>
        </section>

        <section class="card">
            <h2>Update OS Info (Demo)</h2>
            <form method="post" action="">
                <label for="os_name">OS Name</label>
                <input id="os_name" name="os_name" value="<?= $osName ?>" maxlength="128" />

                <label for="os_version">OS Version</label>
                <input id="os_version" name="os_version" value="<?= $osVersion ?>" maxlength="128" />

                <input type="hidden" name="csrf" value="<?= $_SESSION['csrf_token'] ?>" />
                <button type="submit">Update (Demo)</button>
            </form>
        </section>

        <footer>
            <small>Secure demo UI — outputs are escaped and a simple CSRF token is used.</small>
        </footer>
    </div>
</body>
</html>