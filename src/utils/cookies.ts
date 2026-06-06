export class Cookies {
    static clearKey(arg0: string) {
        throw new Error("Method not implemented.");
    }
    static host = window.location.hostname;
    static set(name: string, value: string, days?: number) {
        let domain: string;
        let expires = '';
        if (days) {
            const date = new Date();
            date.setTime(date.getTime() + days * 24 * 60 * 60 * 1000);
            expires = `; expires=${date.toUTCString()}`;
        }
        if (this.host.split('.')?.length === 1) {
            document.cookie = `${name}=${encodeURIComponent(value)}${expires}; path=/`;
        } else {
            const domainParts = this.host.split('.');
            domainParts.shift();
            domain = domainParts.join('.');
            document.cookie = `${name}=${encodeURIComponent(value)}${expires}; path=/; domain=.${domain}`;
        }
    }
    static get(): Record<string, string> {
        return document.cookie
            .split(';')
            .map((cookie) => cookie.split('='))
            .reduce((accumulator, [key, value]) => {
                if (key.trim()) {
                    return { ...accumulator, [key.trim()]: decodeURIComponent(value || '') };
                }
                return accumulator;
            }, {} as Record<string, string>);
    }
    static clear() {
        const host = window.location.hostname;
        const domainParts = host.split('.');
        domainParts.shift();
        const domain = domainParts.join('.');

        document.cookie.split(';').forEach((cookie) => {
            const key = cookie.split('=')[0].trim();
            document.cookie = `${key}=; expires=${new Date(0).toUTCString()}; path=/`;
        });

        if (domain) {
            document.cookie.split(';').forEach((cookie) => {
                const key = cookie.split('=')[0].trim();
                document.cookie = `${key}=; expires=${new Date(0).toUTCString()}; path=/; domain=.${domain}`;
            });
        }
        window.localStorage.clear();
    }
}

export default Cookies;

