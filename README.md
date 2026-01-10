# OS Typing — Güvenlik İyileştirmeleri ve Demo UI

Bu küçük proje için yapılan değişiklikler:

- `db_security.php`:
  - SQL identifier doğrulama ve esktra güvenlik kontrolleri güçlendirildi.
  - Hata loglaması artık SQL içeriğini doğrudan yazmıyor; bunun yerine maskeleme/fingerprint kullanılıyor.
  - PDO için `setSecurePDOOptions` yardımcı fonksiyonu eklendi.

- `os_typing.c`:
  - Sözdizimi hataları düzeltildi, girişler sanitize edildi, geliştirme sırasında sanitizers önerildi.

- `index.php`, `styles.css`:
  - Basit, güvenli demo arayüzü eklendi. Çıkışlar `htmlspecialchars` ile kaçışlıyor ve form POST istekleri için CSRF token kullanılıyor.

- `scripts/checks.ps1`:
  - Windows ortamında hızlı statik kontroller ve derleme denemesi için bir script.

Nasıl test edilir:
- PHP sözdizimi kontrolleri: `powershell -File scripts/checks.ps1`
- Web arayüzü: basit bir PHP sunucusunda çalıştırmak için `php -S 127.0.0.1:8000` ve tarayıcıda `http://127.0.0.1:8000/index.php` ziyaret edin.

PHP lint ve statik analiz (CI):

- CI'da `php -l` yanında **PHPCS** ve **PHPStan** de çalıştırılır. Yerel olarak test etmek için:
  - `php composer.phar require --dev squizlabs/php_codesniffer phpstan/phpstan`
  - `vendor/bin/phpcs --standard=PSR12 --extensions=php -n`
  - `vendor/bin/phpstan analyse -l 5`

- CI manual tetiklemede `fail_on_php_lint` input'unu `true` yaparak lint/analiz hatalarının job'u kırmasını sağlayabilirsiniz.

---

## OS kontrolleri — `os_controlsystem` (🔧)

- Lokal olarak çalıştırma (Linux):
  - `./scripts/tests/run_os_controlsystem_test.sh --config tests/hardening_config.json`
- Lokal olarak çalıştırma (Windows PowerShell):
  - `.\scripts\tests\run_os_controlsystem_test.ps1 -ConfigPath tests/hardening_config.json`

- CI'da manuel tetiklemede (Workflow Dispatch):
  - Repo -> Actions -> CI -> Run workflow
  - `fail_on_os_control` ve `fail_on_hardening` input'larını `true` veya `false` olarak ayarlayabilirsiniz.
  - `fail_on_os_control=true` ayarlandığında, `os_controlsystem` içinde raporlanan mismatch'ler job'u başarısız kılar.

- Raporlama:
  - `os_controlsystem` çıktısı `scripts/tests/os_controlsystem_to_junit.py` ile JUnit XML'e dönüştürülür ve GitHub test-reporter ile yayınlanır.

İpuçları:
- Eğer sadece raporlama istiyorsanız, workflow'u default değerlerle çalıştırın (input'lar `false`).
- Gelişmiş kullanım için, workflow'u branch/protection kuralları veya repo dispatch ile otomatik tetikleyip stricter policy uygulayabilirsiniz.

Güvenlik notları:
- Bu repo örnek / demo amaçlıdır. Gerçek uygulamalarda inputları veri tabanına koymadan önce parametrik sorgular ve ek validasyonlar kullanın.
- Loglama hassas verileri sızdırmamalıdır; bu yüzden SQL gövdeleri maskelenir veya hiç yazılmaz.
