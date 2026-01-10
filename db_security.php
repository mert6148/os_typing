<?php
declare(strict_types=1);
/**
 * DB Security helpers
 * - Identifier validation and quoting for table/column names
 * - Safe integer coercion
 * - Safe query wrapper
 * - PDO hardening helper
 */

namespace App\Database\Config;

use PDO;
use InvalidArgumentException;

class DBSecurity {
    /**
     * Validate a SQL identifier (table, column, schema) against a strict whitelist pattern.
     * Allows letters, numbers and underscore, max length 64.
     */
    public static function isValidIdentifier(string $name): bool {
        return (bool)preg_match('/^[A-Za-z0-9_]{1,64}$/', $name);
    }

    /**
     * Quote an identifier for safe interpolation into SQL. Throws on invalid name.
     * Note: caller should avoid interpolating user-supplied identifiers wherever possible.
     */
    public static function quoteIdentifier(string $name): string {
        if (!self::isValidIdentifier($name)) {
            throw new InvalidArgumentException("Invalid SQL identifier provided");
        }
        // Escape backticks defensively
        $name = str_replace('`', '``', $name);
        return "`$name`";
    }

    /**
     * Safely coerce a value to integer with optional bounds.
     */
    public static function toInt(mixed $value, int $default = 0, ?int $min = null, ?int $max = null): int {
        if (is_bool($value)) {
            return $value ? 1 : 0;
        }
        $i = filter_var($value, FILTER_VALIDATE_INT, FILTER_NULL_ON_FAILURE);
        if ($i === null) return $default;
        if ($min !== null && $i < $min) $i = $min;
        if ($max !== null && $i > $max) $i = $max;
        return $i;
    }

    /**
     * Mask parameter values for logging (strings and numbers replaced with '?').
     */
    private static function maskParamsForLogging(array $params): string {
        $parts = [];
        foreach ($params as $k => $v) {
            $key = is_string($k) ? ':' . $k : (string)$k;
            if (is_string($v)) $parts[] = "{$key}='?'";
            else $parts[] = "{$key}=?";
        }
        return implode(", ", $parts);
    }

    /**
     * Mask SQL for logs by removing literal strings and numbers to avoid leaking sensitive data.
     */
    private static function maskSqlForLogging(string $sql): string {
        // Replace single-quoted strings (handles escaped chars) with '?'
        $masked = preg_replace("/'(?:\\.|[^'\\])*'/", "'?'", $sql);
        // Replace double-quoted strings
        $masked = preg_replace('/"(?:\\.|[^"\\])*"/', '"?"', $masked);
        // Replace numeric literals (ints and floats)
        $masked = preg_replace('/\b\d+(?:\.\d+)?\b/', '?', $masked);
        // Normalize whitespace
        $masked = preg_replace('/\s+/', ' ', $masked);
        return trim($masked);
    }

    /**
     * Hardens common PDO options on a PDO instance.
     */
    public static function setSecurePDOOptions(PDO $pdo): void {
        $pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
        $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    }

    /**
     * Safe wrapper for prepared statements: prepares, executes, and returns statement.
     * On failure logs a masked fingerprint and returns false.
     * @return \PDOStatement|false
     */
    public static function safePrepareExecute(PDO $pdo, string $sql, array $params = [], bool $logSql = false): \PDOStatement|false {
        try {
            $stmt = $pdo->prepare($sql);
            if ($stmt === false) {
                throw new \RuntimeException('Failed to prepare statement');
            }
            $stmt->execute($params);
            return $stmt;
        } catch (\Throwable $e) {
            $fingerprint = substr(sha1($sql . json_encode($params)), 0, 12);
            $msg = "DB safe query error: " . $e->getMessage() . " | FP:" . $fingerprint;
            if ($logSql) {
                $msg .= " | SQL: " . self::maskSqlForLogging($sql) . " | PARAMS: " . self::maskParamsForLogging($params);
            }
            error_log($msg);
            return false;
        }
    }
}

// Usage examples (do not execute at include time):
// \App\Database\Config\DBSecurity::setSecurePDOOptions($pdo);
// $safe = \App\Database\Config\DBSecurity::quoteIdentifier('users');
// $count = $pdo->query("SELECT COUNT(*) FROM $safe")->fetchColumn();
?>