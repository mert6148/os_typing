# ASP.NET Application Configuration

## Database Configuration (appsettings.json)

```json
{
  "ConnectionStrings": {
    "DefaultConnection": "Server=.;Database=OsTypingDb;Integrated Security=true;"
  }
}
```

## Program.cs Configuration

```csharp
using Microsoft.EntityFrameworkCore;
using OsTyping.Data;
using OsTyping.Services;

var builder = WebApplication.CreateBuilder(args);

// Add services
builder.Services.AddDbContext<ApplicationDbContext>(options =>
    options.UseSqlServer(builder.Configuration.GetConnectionString("DefaultConnection")));

builder.Services.AddCors(options =>
{
    options.AddPolicy("AllowSpecificOrigins", policy =>
    {
        policy.WithOrigins("http://localhost:3000")
              .AllowAnyMethod()
              .AllowAnyHeader();
    });
});

builder.Services.AddControllers();
builder.Services.AddSwaggerGen();

// Register services
builder.Services.AddScoped<IHardeningService, HardeningService>();
builder.Services.AddScoped<IOsCheckService, OsCheckService>();

var app = builder.Build();

// Configure middleware
if (app.Environment.IsDevelopment())
{
    app.UseSwagger();
    app.UseSwaggerUI();
}

app.UseHttpsRedirection();
app.UseCors("AllowSpecificOrigins");
app.UseAuthentication();
app.UseAuthorization();

app.MapControllers();
app.Run();
```

## Entity Framework Configuration

```csharp
using Microsoft.EntityFrameworkCore;
using OsTyping.Models;

namespace OsTyping.Data
{
    public class ApplicationDbContext : DbContext
    {
        public ApplicationDbContext(DbContextOptions<ApplicationDbContext> options) 
            : base(options) { }

        public DbSet<HardeningCheck> HardeningChecks { get; set; }
        public DbSet<SystemMetric> SystemMetrics { get; set; }
        public DbSet<AuditLog> AuditLogs { get; set; }

        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
            base.OnModelCreating(modelBuilder);

            // Configure entities
            modelBuilder.Entity<HardeningCheck>()
                .HasKey(h => h.Id);
            
            modelBuilder.Entity<HardeningCheck>()
                .Property(h => h.CreatedAt)
                .HasDefaultValueSql("GETUTCDATE()");

            // Configure indexes
            modelBuilder.Entity<AuditLog>()
                .HasIndex(a => a.Timestamp)
                .IsDescending();
        }
    }
}
```

## Dependency Injection Configuration

```csharp
// Service registration in Program.cs
builder.Services.AddScoped<IHardeningService, HardeningService>();
builder.Services.AddScoped<IOsCheckService, OsCheckService>();
builder.Services.AddSingleton<ICacheService, MemoryCacheService>();
builder.Services.AddHttpClient<IExternalApiService, ExternalApiService>();

// Using in controllers
[ApiController]
[Route("api/[controller]")]
public class HardeningController : ControllerBase
{
    private readonly IHardeningService _service;

    public HardeningController(IHardeningService service)
    {
        _service = service;
    }

    [HttpGet("{id}")]
    public async Task<ActionResult<HardeningCheckDto>> GetCheck(int id)
    {
        var result = await _service.GetCheckAsync(id);
        if (result == null)
            return NotFound();
        return Ok(result);
    }
}
```

## Authentication & Authorization

```csharp
// JWT Configuration
builder.Services.AddAuthentication(JwtBearerDefaults.AuthenticationScheme)
    .AddJwtBearer(options =>
    {
        options.TokenValidationParameters = new TokenValidationParameters
        {
            ValidateIssuerSigningKey = true,
            IssuerSigningKey = new SymmetricSecurityKey(Encoding.UTF8.GetBytes(
                builder.Configuration["Security:Jwt:Key"])),
            ValidateIssuer = true,
            ValidIssuer = builder.Configuration["Security:Jwt:Issuer"],
            ValidateAudience = true,
            ValidAudience = builder.Configuration["Security:Jwt:Audience"],
            ValidateLifetime = true
        };
    });

builder.Services.AddAuthorization(options =>
{
    options.AddPolicy("AdminOnly", policy =>
        policy.RequireClaim(ClaimTypes.Role, "Admin"));
    
    options.AddPolicy("HardeningRead", policy =>
        policy.RequireClaim("Permission", "hardening:read"));
});
```

## Middleware Configuration

```csharp
// Error handling middleware
app.UseMiddleware<ExceptionHandlingMiddleware>();

// Logging middleware
app.UseMiddleware<RequestLoggingMiddleware>();

// HTTPS redirect
app.UseHttpsRedirection();

// CORS
app.UseCors("AllowSpecificOrigins");

// Authentication/Authorization
app.UseAuthentication();
app.UseAuthorization();

// Response compression
app.UseResponseCompression();

// Custom middleware
app.Use(async (context, next) =>
{
    context.Response.Headers.Add("X-Content-Type-Options", "nosniff");
    context.Response.Headers.Add("X-Frame-Options", "DENY");
    await next();
});
```

## Testing Configuration (xUnit)

```csharp
using Xunit;
using Moq;
using OsTyping.Services;

public class HardeningServiceTests
{
    private readonly Mock<IHardeningRepository> _mockRepository;
    private readonly HardeningService _service;

    public HardeningServiceTests()
    {
        _mockRepository = new Mock<IHardeningRepository>();
        _service = new HardeningService(_mockRepository.Object);
    }

    [Fact]
    public async Task GetCheck_WithValidId_ReturnsCheck()
    {
        // Arrange
        var checkId = 1;
        var expectedCheck = new HardeningCheck { Id = checkId, Name = "Test" };
        _mockRepository.Setup(r => r.GetByIdAsync(checkId))
            .ReturnsAsync(expectedCheck);

        // Act
        var result = await _service.GetCheckAsync(checkId);

        // Assert
        Assert.NotNull(result);
        Assert.Equal(checkId, result.Id);
    }
}
```
