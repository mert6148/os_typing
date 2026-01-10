# Deploy Notları — OS Typing (Güvenlik Önerileri)

Bu doküman deploy öncesi ve sonrası dikkat edilmesi gereken güvenlik ve operasyon adımlarını listeler.

1. PHP Konfigürasyonu
   - `display_errors` üretim ortamında **kapalı** olmalıdır.
   - `session.cookie_secure = 1` ve `session.cookie_httponly = 1` ayarlarını kullanın.
   - PHP sürümünü güncel tutun (LTS güvenlik sürümü önerilir).

2. PDO Güvenliği
   - Bağlantı kurulduktan sonra `DBSecurity::setSecurePDOOptions($pdo)` çağrısını yapın.
   - Parametrik sorgular kullanın; hiçbir zaman kullanıcı girdisini doğrudan SQL'e eklemeyin.

3. Loglama ve Gizlilik
   - Hata logları hassas verileri içermemeli; `db_security.php` artık SQL gövdelerini maskeleyerek/özetleyerek loglar.
   - Log rotasyonu ve erişim izinlerini düzgün yapılandırın.

4. Web Güvenliği
   - HTTPS zorunlu olsun (HSTS başlığı ekleyin).
   - İçerik Güvenlik Politikası (CSP) ekleyin; minimal 'self' politikası başlangıç için uygundur.
   - XSS ve CSRF'ye karşı koruma: tüm çıktı `htmlspecialchars` ile kaçışlanmalı ve form postları CSRF token ile doğrulanmalıdır.

5. İzinler ve Dosya Sistemi
   - PHP dosyalarına uygun dosya izinleri verin (web kullanıcı sadece okuma/yürütme, yazma yok).
   - `session` klasörleri ve log dosyaları için erişim sınırlamaları uygulayın.

6. Test ve Süreklilik
   - CI pipeline'ınıza `php -l` ve unit testleri ekleyin.
   - Düzenli güvenlik taramaları (SAST/DAST) ve bağımlılık taramaları yapın.

---

### CI: OS kontrolleri ve `FAIL_ON_OS_CONTROL` (🔧)

- Bu proje, `scripts/c-c++/os_controlsystem` aracı ve test sarmalayıcıları ile **konfigürasyona dayalı** OS kontrolleri (sysctl değerleri, systemd unit ExecStart, firewall port'ları vb.) çalıştırır.
- CI varsayılan davranışı: kontrol sonuçlarını **raporlar** ama job'u bozmaz (non-fatal).
- Eğer kontrol sonuçlarının CI'da **fatal** olmasını isterseniz, workflow veya job seviyesinde ortam değişkeni olarak `FAIL_ON_OS_CONTROL=1` ayarlayın; bu durumda `os_controlsystem` mismatch'leri job'u başarısız kılar.

Kısa örnekler:

- GitHub Actions (örnek snippet):

  env:
    FAIL_ON_OS_CONTROL: "1"  # veya "0" / boş bırakın (default non-fatal)

- Lokal çalıştırma:
  - Linux: `./scripts/tests/run_os_controlsystem_test.sh --config tests/hardening_config.json`
  - Windows (PowerShell): `.\scripts\tests\run_os_controlsystem_test.ps1 -ConfigPath tests/hardening_config.json`

- Çıktı örnekleri:
  - `[sysctl:net.ipv4.ip_forward] OK`
  - `[sysctl:net.ipv4.ip_forward] MISMATCH expected=0 actual=1`

- CI entegrasyonu:
  - Loglar `scripts/tests/os_controlsystem_to_junit.py` ile JUnit XML'e dönüştürülür ve `dorny/test-reporter` ile yayınlanır; böylece her anahtar için ayrı test case olarak rapor alınır.

İpuçları:
  - Raporlama istiyor ancak CI'yi kırmak istemiyorsanız `FAIL_ON_OS_CONTROL`'ü boş bırakın (default false).
  - Daha katı policy isterseniz workflow-level env ayarı ile tüm run'larda strict davranışı zorlayabilirsiniz.

---

7. Geliştirme Araçları
   - Geliştirme ortamında address sanitizer, ASAN ve UBSAN ile C kodunu test edin.
   - CI'da `-Wall -Wextra -Werror` compiler bayrakları ile derleme yapmayı düşünün.

8. Geliştirme Sertifikaları (HTTPS - Local)
   - Geliştirme için self-signed TLS sertifikası oluşturmak üzere `scripts/generate_cert.ps1` eklendi.
   - Üretim ortamı için Let's Encrypt / ACME tabanlı sertifika sağlayıcıları kullanın (Certbot veya uygunsa ACME client).
   - Sertifika özel anahtarlarını asla kaynak koduna eklemeyin; anahtarlar güvenli bir secret manager veya sunucu dosya sistemi içinde saklanmalıdır.

--
Gerekirse bu sayfayı projenizin deploy checklist'ine ekleyelim veya bir CI job'a çevirip otomatik kontrol sağlayalım.

## OS Konfigürasyonları (Linux / Windows)

- Linux:
  - `deploy/systemd/os_typing.service` — service unit şablonu. Kopyalayın ve `ExecStart` ile `User` değerlerini gerçek ortamınıza göre güncelleyin.
  - `deploy/linux/sysctl-os_typing.conf` — conservative sysctl ayarları. Sisteme uygulamak için `/etc/sysctl.d/99-os_typing.conf` altına koyup `sudo sysctl --system` çalıştırın.
  - `deploy/linux/ufw-setup.sh` — UFW tabanlı basit firewall kurulum scripti.
  - Kontroller için `scripts/os_hardening_check.sh` eklendi.

- Windows:
  - `deploy/windows/hardening.ps1` — temel PowerShell hardening görevleri (Firewall, Defender, SMBv1 devre dışı bırakma). Yönetici ayrıcalıklarıyla çalıştırın.

Örnek uygulama:

1. Kopya systemd unit'i `/etc/systemd/system/os_typing.service` olarak yerleştirin, `ExecStart`'ı ayarlayın ve `sudo systemctl daemon-reload && sudo systemctl enable --now os_typing` ile başlatın.
2. `sudo cp deploy/linux/sysctl-os_typing.conf /etc/sysctl.d/99-os_typing.conf && sudo sysctl --system`
3. `sudo ./deploy/linux/ufw-setup.sh --ssh-port 22 --allow-port 12345`
4. Windows için `deploy/windows/hardening.ps1`'i yönetici haklarıyla çalıştırın ve sistemle uyumlu olduğundan emin olun.

> **Not:** Bu ayarlar örnek ve muhafazakar varsayımlardır; canlı üretim sisteminize uygulamadan önce test ortamında doğrulayın. Eğer isterseniz ben bu adımları otomatikleştiren Ansible playbook veya DSC scripti hazırlayabilirim.