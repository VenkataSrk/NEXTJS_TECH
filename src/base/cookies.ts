export class Cookies {
  static host = window.location.hostname;
  static set(name: any, value: any, days: any) {
    let domain, domainParts, date: any, expires;
    if (days) {
      date = new Date();
      date.setTime(date.getTime() + days * 24 * 60 * 60 * 1000);
      expires = "; expires=" + date.toGMTString();
    } else {
      expires = "";
    }
    if (this.host.split(".").length === 1) {
      document.cookie = name + "=" + value + expires + "; path=/";
    } else {
      domainParts = this.host.split(".");
      domainParts.shift();
      domain = domainParts.join(".");
      document.cookie =
        name + "=" + value + expires + "; path=/; domain=" + domain;
    }
  }
  static get = () => {
    return document.cookie
      .split(";")
      .map((cookie) => cookie.split("="))
      .reduce((accumulator, [key, value]) => {
        if (key.trim())
          return { ...accumulator, [key.trim()]: decodeURIComponent(value) };
        else return "";
      }, {});
  };

  static clear = () => {
    const host = window.location.hostname;
    const domainParts = host.split(".");
    domainParts.shift();
    const domain = domainParts.join(".");
    document.cookie.split(";").forEach((ck) => {
      // document.cookie = 
      // ck
      //   .replace(/^ +/, "")
      //   .replace(
      //     /=.*/,
      //     "=;expires=" + new Date().toUTCString() + ";path=/" + domain
      //   );
      const name = ck.split("=")[0].trim();

      // Clear host-only
      document.cookie = `${name}=;expires=Thu, 01 Jan 1970 00:00:00 GMT;path=/`;

      // Clear domain-wide
      document.cookie = `${name}=;expires=Thu, 01 Jan 1970 00:00:00 GMT;path=/;domain=.${domain}`;
    });

    window.localStorage.clear();
  };

  static clearKey = (name: string) => {
    const host = window.location.hostname;
    const domainParts = host.split(".");
    domainParts.shift();
    const domain = domainParts.join(".");

    // Clear host-only cookie
    document.cookie = `${name}=;expires=Thu, 01 Jan 1970 00:00:00 GMT;path=/`;

    // Clear domain-wide cookie (e.g., for subdomains)
    document.cookie = `${name}=;expires=Thu, 01 Jan 1970 00:00:00 GMT;path=/;domain=.${domain}`;
  };
}
