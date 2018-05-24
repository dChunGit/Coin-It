package com.simplex.coin2credit;

import android.arch.persistence.room.ColumnInfo;
import android.arch.persistence.room.Entity;
import android.arch.persistence.room.PrimaryKey;

@Entity(tableName = "transactionDB")
public class Transaction {
    @PrimaryKey(autoGenerate = true)
    private int uid;

    @ColumnInfo(name = "date")
    private String date;

    @ColumnInfo(name = "amount")
    private String amount;

    public int getUid() {
        return uid;
    }

    public void setUid(int uid) {
        this.uid = uid;
    }

    public String getDate() {
        return date;
    }

    public void setDate(String date) {
        this.date = date;
    }

    public String getAmount() {
        return amount;
    }

    public void setAmount(String amount) {
        this.amount = amount;
    }
}
