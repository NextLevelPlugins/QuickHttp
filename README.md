# 🧩 QuickHttp

**QuickHttp** is a lightweight, cross-version Unreal Engine plugin that brings full **HTTP + JSON** capabilities to Blueprints.  
Easily send REST API requests, build JSON payloads, and handle responses — **no C++ required**.

---

## 🚀 Features

- 🔹 **Async HTTP JSON Request Node** — Send `GET`, `POST`, `PUT`, `PATCH`, or `DELETE` requests directly from Blueprints.  
- 🔹 **Built-in Retry Logic & Timeout Watchdog** — Handles network instability with exponential backoff and safe cancellation tokens.  
- 🔹 **Blueprint JSON Builder** — Quickly create complex, nested JSON payloads without writing code.  
- 🔹 **Pretty Print JSON Utility** — Format and debug JSON responses with a single Blueprint node.  
- 🔹 **Cross-Version Compatible** — Fully supports **Unreal Engine 4.25 → 5.x**.

---

## 🛠️ Installation

1. Copy the `QuickHttp` folder into your project’s `Plugins/` directory.  
2. Regenerate your project files (`.uproject` → *Right-click → Generate Visual Studio project files*).  
3. Rebuild your project and enable **QuickHttp** in the Unreal Editor plugin list.  

✅ You’re ready to send REST API calls from Blueprints!

---

## 🧠 Example Blueprint Flow

```text
New JSON Builder →
  Set String("username", "NextLevelPlugins") →
  Set Int("score", 2500) →
  To JSON String →
  HTTP JSON Request (POST https://api.example.com/submit)
```

---
 
### 🏢 About NextLevelPlugins

**NextLevelPlugins LLC** builds high-quality Unreal Engine plugins that empower developers to create faster, smarter, and more efficiently.  
We focus on tools that simplify workflows — from networking and automation to design systems and gameplay frameworks.

🌐 [nextlevelplugins.com](https://www.nextlevelplugins.com)  
🧩 Follow us for updates, new releases, and development insights.

---
